#include "imagesolver.hpp"
#include "math_tree.hpp"
#include "node.hpp"
#include "task_buffer.hpp"

#include "switches.hpp"

using namespace std;
using boost::logic::tribool;
using boost::thread;

ImageSolver::ImageSolver(FabVars& v)
    : Solver(v)
{
    // Nothing to do here.
}

ImageSolver::ImageSolver(MathTree* tree, FabVars& v)
    : Solver(tree, v)
{
    // Nothing to do here.
}

///////////////////////////////////////////////////////////////////////////////

// Evaluate a single region, either with point-by-point evaluation or
// interval math + recursion.  Operates in a single thread and spawns
// no children.
void ImageSolver::evaluate_region(Region r)
{  
    // For sufficiently small fractions of the space, do a
    // point-by-point evaluation rather than recursing.
    if (r.volume <= v.min_volume) {
        evaluate_points(r);
        v.pb.update(r.volume);
        return;
    }
    
    // Convert from pixel regions to intervals
    FabInterval X = v.x(r.imin, r.imax);
    FabInterval Y = v.y(r.jmin, r.jmax);
    FabInterval Z = v.z(r.kmin, r.kmax);

    tree->eval(X, Y, Z);
    
    // If the result was unambiguous, then fill in that part
    // of the image, then return.
    if (v.mode == SOLVE_BOOL) {
        tribool result = tree->root->result_bool;
        if (result)
            v.fill(r);
        if (!indeterminate(result)) {
            v.pb.update(r.volume);
            return;
        }
    } else if (v.mode == SOLVE_REAL) {
        tribool result = tree->root->result_interval <= FabInterval(0);

        if (result)
            v.fill(r);
        if (!indeterminate(result)) {
            v.pb.update(r.volume);
            return;
        }   
    } else if (v.mode == SOLVE_RGB) {
        int result = tree->root->result_color;
        if (result != -1) {
            v.fill(r, result & 255,
                     (result >> 8) & 255,
                     (result >> 16) & 255);
            v.pb.update(r.volume);
            return;
        }
    }

    // Split the region and recurse
    list<Region> subregions = r.split();

#if CULL_Z
    if (v.nk > 1)
        cull_regions(subregions);
    if (subregions.size() == 0)
        return;
#endif

#if PRUNE_TREE
    tree->push();
#endif

    list<Region>::iterator it;

/*
    // Implementing a full-on thread pool seems to slow it down
    Region mine = subregions.front();
    subregions.pop_front();
    for (it = subregions.begin(); it != subregions.end(); ++it)
        if (!task_buffer->add(*it, tree))
            evaluate_region(*it);
    evaluate_region(mine);

    tree->wait_for_clones();
*/

    for (it = subregions.begin(); it != subregions.end(); ++it)
        evaluate_region(*it);


#if PRUNE_TREE
    tree->pop();
#endif
}

///////////////////////////////////////////////////////////////////////////////

// Evalutes a region full of points, one at a time.
void ImageSolver::evaluate_points(Region r)
{
    for (int k = r.kmax - 1; k >= r.kmin; --k)
    {
    
        // Calculate Z coordinate and height-map scaling.
        float Z = v.z(k);
        float scale = v.scale(k);
        
        for (int i = r.imin; i < r.imax; ++i)
        { // X loop
            float X = v.x(i);
            
            for (int j = r.jmin; j < r.jmax; ++j)
            { // Y loop
                float Y = v.y(j);
                
                // If we can't brighten the image, skip this point.
                if ((v.mode == SOLVE_BOOL || v.mode == SOLVE_REAL)
                     && scale <= v.intensity[v.nj - j - 1][i])
                    continue;

                // Evaluate tree                        
                tree->eval(X, Y, Z);
                
                // Fill in greyscale image
                if ((v.mode == SOLVE_BOOL && tree->root->result_bool) ||
                    (v.mode == SOLVE_REAL && tree->root->result_float <= 0))
                {
                        v.intensity[v.nj - j - 1][i] = scale;
                }                
                // Fill in color image
                else if (v.mode == SOLVE_RGB) {
                    int result = tree->root->result_color;
                    
                    // Extract colors from bit-field
                    unsigned char r = (result & 255) * scale,
                                  g = ((result >> 8) & 255) * scale,
                                  b = ((result >> 16) & 255) * scale;
                                  
                    // Only brighten the image.
                    if (r > v.red[v.nj - j - 1][i])
                        v.red[v.nj - j - 1][i] = r;
                    if (g > v.green[v.nj - j - 1][i])
                        v.green[v.nj - j - 1][i] = g;
                    if (b > v.blue[v.nj - j - 1][i])
                        v.blue[v.nj - j - 1][i] = b;
                }
                
            } // Y loop
        } // X loop
    } // Z loop
}

///////////////////////////////////////////////////////////////////////////////

// Removes any regions that cannot change the image.
void ImageSolver::cull_regions(list<Region>& subregions)
{
    list<Region>::iterator it = subregions.begin();
    
    if (v.mode == SOLVE_BOOL || v.mode == SOLVE_REAL) {
        while (it != subregions.end()) {
            int scale = v.scale(it->kmax - 1);
            bool cull = true;
            for (int i = it->imin; i < it->imax && cull; ++i)
                for (int j = it->jmin; j < it->jmax && cull; ++j)
                    if (v.intensity[v.nj - j - 1][i] < scale)
                        cull = false;
            if (cull) {
                v.pb.update(it->volume);
                it = subregions.erase(it);
            }
            else
                ++it;
        }
    } else if (v.mode == SOLVE_RGB) {
        while (it != subregions.end()) {
            int scale = v.scale(it->kmax - 1) * 255;
            bool cull = true;
            for (int i = it->imin; i < it->imax && cull; ++i)
                for (int j = it->jmin; j < it->jmax && cull; ++j)
                    if (scale > v.red[v.nj - j - 1][i] ||
                        scale > v.green[v.nj - j - 1][i] ||
                        scale > v.blue[v.nj - j - 1][i])
                        cull = false;
            if (cull) {
                v.pb.update(it->volume);
                it = subregions.erase(it);
            } else
                ++it;
        }
    }
}


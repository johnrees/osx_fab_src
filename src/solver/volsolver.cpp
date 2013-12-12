#include "volsolver.hpp"
#include "math_tree.hpp"
#include "node.hpp"

#include "switches.hpp"

#include <iostream>
#include <cstdio>

using namespace std;
using boost::logic::tribool;
using boost::thread;

///////////////////////////////////////////////////////////////////////////////

VolSolver::VolSolver(FabVars& v)
    : Solver(v)
{
    // Nothing to do here.
}

VolSolver::VolSolver(MathTree* tree, FabVars& v)
    : Solver(tree, v)
{
    // Nothing to do here.
}

void VolSolver::save()
{
    v.add_volume(volume);
}

// Evaluate a single region, either with point-by-point evaluation or
// interval math + recursion.  Operates in a single thread and spawns
// no children.
void VolSolver::evaluate_region(Region r)
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
    tribool result;
    if (v.mode == SOLVE_BOOL)
        result = tree->root->result_bool;
    else if (v.mode == SOLVE_REAL)
        result = tree->root->result_interval < FabInterval(0,0);
    
    if (result)
        volume += r.volume;
        
    if (!indeterminate(result)) {
        v.pb.update(r.volume);
        return;
    }

    // Split the region and recurse
    list<Region> subregions = r.split();

#if PRUNE_TREE
    tree->push();
#endif

    list<Region>::iterator it;
    for (it = subregions.begin(); it != subregions.end(); ++it)
        evaluate_region(*it);

#if PRUNE_TREE
    tree->pop();
#endif
}

///////////////////////////////////////////////////////////////////////////////
void VolSolver::evaluate_points(Region r)
{
    for (int k = r.kmax - 1; k >= r.kmin; --k)
    {
    
        // Calculate Z coordinate and height-map scaling.
        float Z = v.z(k);
        
        for (int i = r.imin; i < r.imax; ++i)
        { // X loop
            float X = v.x(i);
            
            for (int j = r.jmin; j < r.jmax; ++j)
            { // Y loop
                float Y = v.y(j);

                // Evaluate tree                        
                tree->eval(X, Y, Z);
                
                // Fill in greyscale image
                if ((v.mode == SOLVE_BOOL && tree->root->result_bool) ||
                    (v.mode == SOLVE_REAL && tree->root->result_float <= 0))
                {
                        volume++;
                }
                
            } // Y loop
        } // X loop
    } // Z loop
}
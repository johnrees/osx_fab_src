#include "raycaster.hpp"
#include "math_tree.hpp"
#include "node.hpp"
#include "task_buffer.hpp"

#include "switches.hpp"

using namespace std;
using boost::logic::tribool;
using boost::thread;

Raycaster::Raycaster(FabVars& v)
    : Solver(v)
{
    // Nothing to do here.
}

Raycaster::Raycaster(MathTree* tree, FabVars& v)
    : Solver(tree, v)
{
    // Nothing to do here.
}

///////////////////////////////////////////////////////////////////////////////

// Evaluate a single region, either with point-by-point evaluation or
// interval math + recursion.  Operates in a single thread and spawns
// no children.
void Raycaster::evaluate_region(Region r)
{    
    
    FabInterval X = v.x(r.imin, r.imax-1);
    FabInterval Y = v.y(r.jmin, r.jmax-1);
    FabInterval Z = v.z(r.kmin, r.kmax-1);

    tree->eval(X, Y, Z);

    // If the result was unambiguous, then fill in that part
    // of the image, then return.
    tribool result = tree->root->result_interval <= FabInterval(0);

    if (result) v.fill(r);
    if (!indeterminate(result)) return;

    // Use Newton's method on the distance metric to find
    // an upper bound on the front of the object.
    
    // If we've hit voxel level, use floating-point evaluation
    // when evaluating Newton's method.
    if (r.ni * r.nj <= 1) {
        float x = v.x(r.imin);
        float y = v.y(r.jmin);
        
        float prev_k = r.kmax;
        float k = prev_k;
        do {
            prev_k = k;
            
            tree->eval(x, y, v.z(k));
            float d1 = tree->root->result_float;
            
            tree->eval(x, y, v.z(k - 1));
            float d2 = tree->root->result_float;
            
            float slope = d1 - d2;
            k = k - d1 / slope;
            
        } while (abs(k - prev_k) > 0.5);
        
        r.kmax = k;
        v.fill(r);
        
        return;
    }
    

    // Otherwise, use interval evaluation on the xy region.
    float prev_k = r.kmax;
    float k = prev_k;
    do {
        prev_k = k;

        tree->eval(X, Y, v.z(k));
        float d1 = tree->root->result_interval.lower();
        
        tree->eval(X, Y, v.z(k - 1));
        float d2 = tree->root->result_interval.lower();
        
        float slope = d1 - d2;
        k = k - d1 / slope;
    } while (abs(k - prev_k) > 0.5);
    r.kmax = k;
    
    list<Region> subregions = r.split_xy(2);
    list<Region>::iterator it;

    for (it = subregions.begin(); it != subregions.end(); ++it)
        evaluate_region(*it);
}
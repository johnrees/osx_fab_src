#include "edgesolver.hpp"
#include "math_tree.hpp"
#include "node.hpp"

#include "switches.hpp"


#include <iostream>
#include <cstdio>

using namespace std;
using boost::logic::tribool;
using boost::logic::indeterminate;
using boost::thread;

const Vec3f OFFSETS[] = {
    Vec3f(0, 0), Vec3f(1, 0), Vec3f(1, 1), Vec3f(0, 1)
};

const int EDGE_MAP[16][2][2] = {
    {{-1,-1}, {-1,-1}}, // ----
    {{ 0, 1}, { 0, 3}}, // ---0
    {{ 1, 2}, { 1, 0}}, // --1-
    {{ 1, 2}, { 0, 3}}, // --10
    {{ 2, 3}, { 2, 1}}, // -2--
    {{-2,-2}, {-2,-2}}, // -2-0
    {{ 2, 3}, { 1, 0}}, // -21-
    {{ 2, 3}, { 0, 3}}, // -210
    
    {{ 3, 0}, { 3, 2}}, // 3---
    {{ 0, 1}, { 3, 2}}, // 3--0
    {{-2,-2}, {-2,-2}}, // 3-1-
    {{ 1, 2}, { 3, 2}}, // 3-10
    {{ 3, 0}, { 2, 1}}, // 32--
    {{ 0, 1}, { 2, 1}}, // 32-0
    {{ 3, 0}, { 1, 0}}, // 321-
    {{-1,-1}, {-1,-1}} // 3210
};


///////////////////////////////////////////////////////////////////////////////

EdgeSolver::EdgeSolver(FabVars& v)
    : Solver(v), paths(v.decimation_error)
{
    // Nothing to do here.
}

EdgeSolver::EdgeSolver(MathTree* tree, FabVars& v)
    : Solver(tree, v), paths(v.decimation_error)
{
    // Nothing to do here.
}

void EdgeSolver::save()
{
    v.add_paths(paths);
}

// Evaluate a single region, either with point-by-point evaluation or
// interval math + recursion.  Operates in a single thread and spawns
// no children.
void EdgeSolver::evaluate_region(Region r)
{  
    // For sufficiently small fractions of the space, do a
    // point-by-point evaluation rather than recursing.
    if (r.volume == 1) {
        evaluate_voxel(r);
        v.pb.update(r.volume);
        return;
    }
    
    // Convert from pixel regions to intervals
    FabInterval X = v.x(r.imin, r.imax);
    FabInterval Y = v.y(r.jmin, r.jmax);
    FabInterval Z = v.z(r.kmin, r.kmax);

    tree->eval(X, Y, Z);
    
    // If the result was unambiguous, then we don't care since it
    // is either entirely inside or outside the image. 
    bool result = false;
    if (v.mode == SOLVE_BOOL)
        result = !indeterminate(tree->root->result_bool);
    else if (v.mode == SOLVE_REAL)
        result = !indeterminate(tree->root->result_interval < FabInterval(0));
        
    if (result) {
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

// Evaluate a single rectangle, using interpolation to smooth the edges.
void EdgeSolver::evaluate_voxel(Region r)
{
    Vec3f corner(r.imin, r.jmin, r.kmin);
    Vec3f v1, v2; // edge vertices
    
    Vec3f vertices[4];
    for (int i = 0; i < 4; ++i)
        vertices[i] = corner + OFFSETS[i];
        
    int lookup = 0;
    for (int i = 3; i >= 0; --i) {
        lookup <<= 1;
        Vec3f pos = vertices[i];
        
        if (point_cache.find(pos) == point_cache.end()) {
            tree->eval(v.x(pos.x), v.y(pos.y), v.z(pos.z));

            if (v.mode == SOLVE_BOOL)
                point_cache[pos] = tree->root->result_bool;
            else if (v.mode == SOLVE_REAL)
                point_cache[pos] = tree->root->result_float < 0;
        }
        
        if (point_cache[pos])
            lookup++;
    }    
    
    if (EDGE_MAP[lookup][0][0] == -1)
        return;
    if (EDGE_MAP[lookup][0][0] == -2)
        return;
    
    v1 = interpolate(vertices[EDGE_MAP[lookup][0][0]],
                     vertices[EDGE_MAP[lookup][0][1]]);
    v2 = interpolate(vertices[EDGE_MAP[lookup][1][0]],
                     vertices[EDGE_MAP[lookup][1][1]]);
    paths += Path(v1, v2);
           
}

// Interpolates between a full and empty point, using caching to
// reduce the number of lookups required.
Vec3f EdgeSolver::interpolate(Vec3f filled, Vec3f empty)
{
    std::map<Edge, Vec3f>::iterator it;
    it = edge_cache.find(Edge(filled, empty));
    if (it != edge_cache.end())
        return it->second;
        
    float step_size = 0.25;
    float interp = 0.5;
    Vec3f offset = empty - filled;
    
    for (int i = 0; i < v.quality; ++i) {
        Vec3f pos = filled + offset * interp;
    
        tree->eval(v.x(pos.x), v.y(pos.y), v.z(pos.z));
        if ((v.mode == SOLVE_BOOL && tree->root->result_bool) || 
            (v.mode == SOLVE_REAL && tree->root->result_float < 0))
            interp += step_size;
        else
            interp -= step_size;
        step_size /= 2;
    }
    edge_cache[Edge(filled, empty)] = filled + offset * interp;
    
    return filled + offset * interp;
}
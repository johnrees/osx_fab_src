#include "trisolver.hpp"
#include "math_tree.hpp"
#include "node.hpp"

#include "switches.hpp"

#include <iostream>
#include <cstdio>

using namespace std;
using boost::logic::tribool;
using boost::thread;

const Vec3f VERTEX_LOOP[] = {
    Vec3f(1, 1, 0), Vec3f(1, 0, 0), Vec3f(1, 0, 1),
    Vec3f(0, 0, 1), Vec3f(0, 1, 1), Vec3f(0, 1, 0),
    Vec3f(1, 1, 0)
};

// Based on which vertices are filled, this map tells you which
// edges to interpolate between when forming zero, one, or two
// triangles.
// (filled vertex is first in the pair)
const int EDGE_MAP[16][2][3][2] = {
    {{{-1,-1}, {-1,-1}, {-1,-1}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // ----
    {{{ 0, 2}, { 0, 1}, { 0, 3}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // ---0
    {{{ 1, 0}, { 1, 2}, { 1, 3}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // --1-
    {{{ 1, 2}, { 1, 3}, { 0, 3}}, {{ 0, 3}, { 0, 2}, { 1, 2}}}, // --10
    {{{ 2, 0}, { 2, 3}, { 2, 1}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // -2--
    {{{ 0, 3}, { 2, 3}, { 2, 1}}, {{ 2, 1}, { 0, 1}, { 0, 3}}}, // -2-0
    {{{ 1, 0}, { 2, 0}, { 2, 3}}, {{ 2, 3}, { 1, 3}, { 1, 0}}}, // -21-
    {{{ 2, 3}, { 1, 3}, { 0, 3}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // -210

    {{{ 3, 0}, { 3, 1}, { 3, 2}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // 3---
    {{{ 3, 2}, { 0, 2}, { 0, 1}}, {{ 0, 1}, { 3, 1}, { 3, 2}}}, // 3--0
    {{{ 1, 2}, { 3, 2}, { 3, 0}}, {{ 3, 0}, { 1, 0}, { 1, 2}}}, // 3-1-
    {{{ 1, 2}, { 3, 2}, { 0, 2}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // 3-10
    {{{ 3, 0}, { 3, 1}, { 2, 1}}, {{ 2, 1}, { 2, 0}, { 3, 0}}}, // 32--
    {{{ 3, 1}, { 2, 1}, { 0, 1}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // 32-0
    {{{ 3, 0}, { 1, 0}, { 2, 0}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // 321-
    {{{-1,-1}, {-1,-1}, {-1,-1}}, {{-1,-1}, {-1,-1}, {-1,-1}}}, // 3210
};


///////////////////////////////////////////////////////////////////////////////

TriSolver::TriSolver(FabVars& v)
    : Solver(v)
{
    // Nothing to do here.
}

TriSolver::TriSolver(MathTree* tree, FabVars& v)
    : Solver(tree, v)
{
    // Nothing to do here.
}

void TriSolver::save()
{
    v.add_triangles(triangles);
}

// Evaluate a single region, either with point-by-point evaluation or
// interval math + recursion.  Operates in a single thread and spawns
// no children.
void TriSolver::evaluate_region(Region r)
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
    
    // If the result was unambiguous, then fill in that part
    // of the image, then return.
    tribool result;
    if (v.mode == SOLVE_BOOL)
        result = tree->root->result_bool;
    else if (v.mode == SOLVE_REAL)
        result = tree->root->result_interval < FabInterval(0,0);
    
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

void TriSolver::evaluate_voxel(Region r)
{
    Vec3f corner(r.imin, r.jmin, r.kmin);
    Vec3f v1, v2, v3; // triangle vertices
    
    for (int t = 0; t < 6; ++t) {
        // Look up the vertex locations for this tetrahedron
        Vec3f vertices[] = {corner, corner + Vec3f(1, 1, 1),
                            corner + VERTEX_LOOP[t], corner + VERTEX_LOOP[t+1]};
             
        // Check which vertices are filled to see how we need to triangulate.   
        unsigned char lookup = 0;
        for (int vertex = 3; vertex >= 0; --vertex) {
            lookup <<= 1;
            Vec3f pos = vertices[vertex];
            
            std::map<Vec3f, bool>::iterator it = point_cache.find(pos);
            if (it == point_cache.end()) {
                tree->eval(v.x(pos.x), v.y(pos.y), v.z(pos.z));
                
                if (v.mode == SOLVE_BOOL)
                    point_cache[pos] = tree->root->result_bool;
                else if (v.mode == SOLVE_REAL)
                    point_cache[pos] = tree->root->result_float < 0;
                    
                if (point_cache[pos])
                    lookup++;
            } else if (it->second)
                lookup++;
        }
        
        // If the triangle map is empty, continue.
        if (EDGE_MAP[lookup][0][0][0] == -1)
            continue;
        
        v1 = interpolate(vertices[EDGE_MAP[lookup][0][0][0]],
                         vertices[EDGE_MAP[lookup][0][0][1]]);
        v2 = interpolate(vertices[EDGE_MAP[lookup][0][1][0]],
                         vertices[EDGE_MAP[lookup][0][1][1]]);
        v3 = interpolate(vertices[EDGE_MAP[lookup][0][2][0]],
                         vertices[EDGE_MAP[lookup][0][2][1]]);
        triangles.push_back(Triangle(v1, v2, v3));

        
        // If there was only one triangle, skip the second face
        if (EDGE_MAP[lookup][1][0][0] == -1)
            continue;
        
        v1 = interpolate(vertices[EDGE_MAP[lookup][1][0][0]],
                         vertices[EDGE_MAP[lookup][1][0][1]]);
        v2 = interpolate(vertices[EDGE_MAP[lookup][1][1][0]],
                         vertices[EDGE_MAP[lookup][1][1][1]]);
        v3 = interpolate(vertices[EDGE_MAP[lookup][1][2][0]],
                         vertices[EDGE_MAP[lookup][1][2][1]]);
        
        triangles.push_back(Triangle(v1, v2, v3));
    }
           
}

// Interpolate between a filled and empty point using binary search
// (after checking in a cache).
Vec3f TriSolver::interpolate(Vec3f filled, Vec3f empty)
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
#ifndef TRISOLVER_H
#define TRISOLVER_H

#include <map>
#include <utility>

#include "solver.hpp"
#include "geometry.hpp"

class TriSolver : public Solver
{
public:
    /*  TriSolver(FabVars& v) 
     *  TriSolver(MathTree* tree, FabVars& v)
     *
     *  Constructs an TriSolver instance.
     */
    TriSolver(FabVars& v);
    TriSolver(MathTree* tree, FabVars& v);
    virtual ~TriSolver() { /* Nothing to do here */ }

    /* virtual void evaluate_region(Region R)
     *  
     *  Evaluate a given region recursively, saving results wherever is
     *  appropriate.
     */
    virtual void evaluate_region(Region R);


    /* void evaluate_voxel(Region R)
     *  
     *  Evaluate a single unit cube.
     */
    void evaluate_voxel(Region R);


    /* virtual void save()
     *  
     *  Saves the results of our calculation by copying paths into our
     *  reference to FabVars v.
     */
    virtual void save();
    
    
    /* Vec3f interpolate(Vec3f filled, Vec3f empty)
     *
     *  Interpolates between a filled and an empty point using binary search.
     *  Values are saved in a cache, and the cache is checked before the
     *  search is run.
     */
    Vec3f interpolate(Vec3f filled, Vec3f empty);

private:
    // MathTree evaluation cached values
    std::map<Vec3f, bool> point_cache;
    
    // Saved edges interpolation values
    std::map<Edge, Vec3f> edge_cache;
    
    // Saved triangles
    std::list<Triangle> triangles;

};
#endif
#ifndef VolSolver_H
#define VolSolver_H

#include <map>
#include <utility>
#include <stdint.h>

#include "solver.hpp"
#include "geometry.hpp"

class VolSolver : public Solver
{
public:
    /*  VolSolver(FabVars& v) 
     *  VolSolver(MathTree* tree, FabVars& v)
     *
     *  Constructs an VolSolver instance.
     */
    VolSolver(FabVars& v);
    VolSolver(MathTree* tree, FabVars& v);
    virtual ~VolSolver() { /* Nothing to do here */ }

    /* virtual void evaluate_region(Region R)
     *  
     *  Evaluate a given region recursively, saving results wherever is
     *  appropriate.
     */
    virtual void evaluate_region(Region R);


    /* void evaluate_points(Region R)
     *  
     *  Evaluate a set of points in a region
     */
    void evaluate_points(Region r);


    /* virtual void save()
     *  
     *  Saves the results of our calculation by copying paths into our
     *  reference to FabVars v.
     */
    virtual void save();


private:
    // Saved volume
    uint64_t volume;

};
#endif
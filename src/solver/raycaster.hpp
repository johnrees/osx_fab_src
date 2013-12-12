#ifndef RAYCASTER_H
#define RAYCASTER_H

#include <list>

#include "solver.hpp"

class Raycaster : public Solver
{
public:
    /*  Raycaster(FabVars& v)
     *  Raycaster(MathTree* tree, FabVars& v)
     *
     *  Constructs an Raycaster instance.
     */
    Raycaster(FabVars& v);
    Raycaster(MathTree* tree, FabVars& v);
    virtual ~Raycaster() { /* Nothing to do here */ }

    /* virtual void evaluate_region(Region R)
     *  
     *  Evaluate a given region recursively, saving results wherever is
     *  appropriate.
     */
    virtual void evaluate_region(Region R);
    
    /* virtual void save()
     *  
     *  Nothing needs to happen here, since evaluate_region dumps data
     *  directly into the image matrix.
     */
    void save() { /* Nothing to do here */ }
};
#endif
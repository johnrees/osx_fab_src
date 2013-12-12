#ifndef SOLVER_H
#define SOLVER_H

#include <boost/thread.hpp>

#include "fabvars.hpp"
#include "region.hpp"
#include "math_tree.hpp"

// Forward declaration
class TaskBuffer;

// Generic solver class.
class Solver
{
public: 

    /*  Solver(MathTree* tree, FabVars& v)
     *  Solver(FabVars& v)
     
     *  Constructs a solver with a given tree (or NULL) and reference to the
     *  global FabVars instance.
     */
    Solver(MathTree* tree, FabVars& v)
        : tree(tree), task_buffer(NULL), v(v) {}
    Solver(FabVars& v)
        : tree(NULL), task_buffer(NULL), v(v) {}
    
    
    virtual ~Solver() { delete tree; }
    

    /* virtual void evaluate_region(Region R) = 0
     *  
     *  Evaluate a given region recursively, saving results wherever is
     *  appropriate.
     */
    virtual void evaluate_region(Region R) = 0;


    /* virtual void save()
     *  
     *  Saves the results of a solver instance's calculation.
     *  Typically, this will involve copying locally saved results into
     *  the solver's FabVars variable.
     */
    virtual void save() = 0;
    
    
    /* void run()
     *  
     *  Runs an instance of the solver from the task buffer.
     */
    void run();
    
    
    /* void set_buffer(TaskBuffer* b)
     *
     *  Sets the queue from which the solver will operate.
     */
    void set_buffer(TaskBuffer* b);
    
protected:
    // The instance of the MathTree associated with this solver.
    MathTree* tree;
    
    // For a thread-pool style solver, this object gives us tasks.
    TaskBuffer* task_buffer;
    
    // A reference to the system's FabVars, where results are stored.
    FabVars& v;
};

#endif
    
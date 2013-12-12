#ifndef THREAD_MANAGER_H
#define THREAD_MANAGER_H

#include "task_buffer.hpp"

template <class SOLVER_TYPE>
class ThreadManager
{
public:

    /* ThreadManager(FabVars& v)
     *
     *  Constructor for ThreadManager.
     */
    ThreadManager(FabVars& v);


    /* void evaluate(MathTree* tree, FabVars& v)
     * 
     *  This function evaluates the math tree on the full region defined in v, writing
     *  solver-dependent output to v.
     */
    void evaluate(MathTree* tree);
    
    
    /* static void make_new_thread()
     *
     *  This function creates a new thread, in which a solver instance calls
     *  Solver::run()
     */  
    void make_new_thread();
                                

    /* void wait_for_threads()
     *
     *  Waits until all of the threads are finished.  As each thread finishes,
     *  this function instructs it to saves its results, then deletes it.
    */
    void wait_for_threads();

private:
    FabVars& v;
    
    typedef std::list<std::pair<boost::thread*, SOLVER_TYPE*> > ThreadList;
    ThreadList threads;
    
    TaskBuffer task_buffer;
};

// Templated implementation file
#include "thread_manager.cpp"

#endif
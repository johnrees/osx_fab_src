// This is an implementation file for the ThreadManager

#include "math_tree.hpp"
#include "switches.hpp"
#include "thread_manager.hpp"
#include "solver.hpp"

using namespace std;
using boost::logic::tribool;
using boost::thread;

///////////////////////////////////////////////////////////////////////////////

template <class SOLVER_TYPE>
ThreadManager<SOLVER_TYPE>::ThreadManager(FabVars& v)
    : v(v), task_buffer(thread::hardware_concurrency())
{
    // Nothing to do here.
}

///////////////////////////////////////////////////////////////////////////////

// Evaluates an entire math tree on the full spatial region.
template <class SOLVER_TYPE>
void ThreadManager<SOLVER_TYPE>::evaluate(MathTree* tree)
{   
    list<Region> regions;
    if (v.projection)
        regions = Region(v).split_xy(task_buffer.size);
    else
        regions = Region(v).split(task_buffer.size);
    
    // Create a task for each region
    list<Region>::iterator it;
    for (it = regions.begin(); it != regions.end(); ++it) {
//        cout << "Added region " << *it << endl;
        if (!task_buffer.add(*it, tree)) {
            cout << "Failed to add initial task to task buffer!" << endl;
            exit(1);
        }
    }
        
    // Create a new thread for all but one of the tasks
    for (unsigned i = 1; i < task_buffer.size; ++i)
        make_new_thread();

    // This thread gets a task as well.
    SOLVER_TYPE* s = new SOLVER_TYPE(v);
    s->set_buffer(&task_buffer);
    s->run();
    delete s;
    
    wait_for_threads();
}

///////////////////////////////////////////////////////////////////////////////

template <class SOLVER_TYPE>
void ThreadManager<SOLVER_TYPE>::make_new_thread()
{
    SOLVER_TYPE* s = new SOLVER_TYPE(v);
    s->set_buffer(&task_buffer);
    thread* newThread = new thread(&Solver::run, s);

    threads.push_back(make_pair(newThread, s));
}

///////////////////////////////////////////////////////////////////////////////

template <class SOLVER_TYPE>
void ThreadManager<SOLVER_TYPE>::wait_for_threads()
{
    while (!threads.empty())
    {
        typename ThreadList::iterator it = threads.begin();
        while(it != threads.end())
            if (it->first->timed_join(boost::system_time()))
            {
                delete it->first;
                delete it->second;
                
                it = threads.erase(it);
            } else {
                ++it;
            }
    }
}
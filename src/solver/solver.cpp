// This is an implementation file for the solver.hpp

// It should not be compiled on its own (since a template needs to
// be instantiated with a particular class).

#include "solver.hpp"

#include "switches.hpp"
#include "task_buffer.hpp"


using namespace std;
using boost::logic::tribool;
using boost::thread;

void Solver::run()
{
    Task task = task_buffer->next();
    task_buffer->hello();
    
    while (task.region.volume != 0) {
        if (task.tree) {
            delete tree;
            tree = task.tree;
        }
        evaluate_region(task.region);
        task = task_buffer->next();
    }

    save();
}

void Solver::set_buffer(TaskBuffer* b)
{
    task_buffer = b;
}
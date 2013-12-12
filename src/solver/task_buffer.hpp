#ifndef TASK_BUFFER_H
#define TASK_BUFFER_H

#include <boost/thread.hpp>

#include "region.hpp"

class MathTree;

// Simple structure containing a task to be solved.
typedef struct Task {
    Task();
    Task(Region region);
    Task(Region region, MathTree* tree);
    
    Region region;
    MathTree* tree;
} Task;


// Slot to store a task and associated metadata in the ring buffer.
typedef struct TaskSlot {
    TaskSlot();
    
    Task task;
    bool ready;
    boost::mutex mutex;
    boost::condition_variable condition;
} TaskSlot;


class TaskBuffer
{
public:
    /* TaskBuffer(unsigned size)
     *
     *  Create a ring buffer of the appropriate size.
     */
    TaskBuffer(unsigned size);
    
    
    /* ~TaskBuffer()
     *
     *  Destructor for the task buffer.
     */
    ~TaskBuffer();
    
    
    /* bool full() const
     *
     *  Checks to see if the task buffer is full in a thread-unsafe
     *  way.  A further check should be run in a thread-safe manner.
     */
    bool full() const;
    
    /* bool add(Region region, MathTree* tree)
     *
     *  Adds a task to the latest available slot, with the provided
     *  region and a clone of the provided tree.  Increments
     *  add_position and unassigned_tasks.
     *
     *  If all slots are full, returns false; otherwise returns true.
     */
    bool add(Region region);
    bool add(Region region, MathTree* tree);
    
    
    /* void hello()
     *
     *  Each active thread should call this function once so that the
     *  task buffer has a count of how many active threads are running.
     */
    void hello();
    
    /* Task next()
     *
     *  Acquires the next task from the list.  If there are no tasks
     *  and no active threads, fill the buffer with null tasks to halt
     *  the process
     *
     *  On start, decrements active_threads; when a task is acquired,
     *  increment active_threads and decrement unassigned_tasks.
     *
     *  Increments read_position.
     */
    Task next();
    
    const unsigned size;
    
private:

    void finish();
    
    boost::mutex add_mutex;
    unsigned add_position;
    
    boost::mutex read_mutex;
    unsigned read_position;
    
    TaskSlot* tasks;
    
    boost::mutex master_mutex;
    unsigned active_threads;
    unsigned unassigned_tasks;
    
    boost::barrier barrier;
};
    
#endif
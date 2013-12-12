#include "math_tree.hpp"
#include "task_buffer.hpp"

#include <iostream>
// using namespace std;

typedef boost::lock_guard<boost::mutex> lock_guard;

Task::Task()
    : tree(NULL)
{
    // Nothing to do here
}

Task::Task(Region region)
    : region(region), tree(NULL)
{
    // Nothing to do here.
}

Task::Task(Region region, MathTree* tree)
    : region(region), tree(tree)
{
    // Nothing to do here
}

TaskSlot::TaskSlot()
    : ready(false)
{
    // Nothing to do here
}


TaskBuffer::TaskBuffer(unsigned size)
    : size(size), add_position(0), read_position(0),
      active_threads(0), unassigned_tasks(0), barrier(size)
{
    tasks = new TaskSlot[size];
}


TaskBuffer::~TaskBuffer()
{
    delete [] tasks;
}


bool TaskBuffer::full() const
{
    return (unassigned_tasks >= size - active_threads);
}

// Add a task to the buffer and notify the corresponding thread.
bool TaskBuffer::add(Region region)
{
    return add(region, NULL);
}

bool TaskBuffer::add(Region region, MathTree* tree)
{

    {   // Check to see if a thread is available for this
        // new unassigned task.
        lock_guard lock(master_mutex);
        if (unassigned_tasks >= size - active_threads)
            return false;
        unassigned_tasks++;
    }


    unsigned pos;
    {   // Figure out which task slot we're about to fill
        lock_guard lock(add_mutex);
        pos = add_position;

        // Increment the add position in the buffer
        add_position = (add_position + 1) % size;
    }

    {   // Fill the task slot
        lock_guard lock(tasks[pos].mutex);
        if (tree)
            tasks[pos].task = Task(region, tree->clone());
        else
            tasks[pos].task = Task(region);
        tasks[pos].ready = true;
//        cout << "Added new task with region " << region << ' ' << tasks[pos].task.tree << endl;
    }
    // Notify anyone that has been waiting on this slot.
    tasks[pos].condition.notify_one();

    return true;
}


// Each thread should call this function once before taking
// data from the task buffer.
void TaskBuffer::hello()
{
    {
        lock_guard lock(master_mutex);
        active_threads++;
    }
    barrier.wait();
}


// Acquire the next task from the buffer.  If there is nothing left, then
// fill the buffer with null tasks to halt execution.
Task TaskBuffer::next()
{
    unsigned pos;
    {   // Figure out the slot from which we'll read
        lock_guard lock(read_mutex);
        pos = read_position;
        read_position = (read_position + 1) % size;
    }

    {   // Mark that this thread is inactive
        lock_guard lock(master_mutex);
        active_threads--;

        // If all threads are inactive and there are no unassigned
        // tasks, then we're done with evaluation.  finish() fills the buffer
        // with null tasks to signal the end of execution.
        if (active_threads == 0 && unassigned_tasks == 0)
            finish();
    }

    Task t;
    {   // Wait until this task slot is filled, then claim the task.
        boost::unique_lock<boost::mutex> lock(tasks[pos].mutex);
        while (!tasks[pos].ready)
            tasks[pos].condition.wait(lock);
        t = tasks[pos].task;
        tasks[pos].ready = false;
    }

    {   // Note that this thread is now active.
        lock_guard lock(master_mutex);
        unassigned_tasks--;
        active_threads++;
    }

    return t;
}


// Populate the task buffer with null tasks, to signal the end of the program.
void TaskBuffer::finish()
{
    for (unsigned i = 0; i < size; ++i) {
        lock_guard lock(tasks[i].mutex);
        tasks[i].task = Task();
        tasks[i].ready = true;
        tasks[i].condition.notify_one();
    }
}
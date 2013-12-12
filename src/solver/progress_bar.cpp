#include <iostream>
#include "progress_bar.hpp"

using namespace std;

ProgressBar::ProgressBar()
    : full(0), progress(0), next_tick(1), bar_length(40)
{
    // Nothing to do here
}

bool ProgressBar::update(uint64_t delta)
{  
    if (!lock.try_lock())
        return false;
    progress += delta;
    lock.unlock();
    
    if (!cout_lock.try_lock())
        return true;

    while (next_tick <= (progress*bar_length) / full) {
        cout << "\r    [";        
        for(unsigned i = 0; i < next_tick; ++i)
            cout << '|';
        for(int i = next_tick; i < bar_length; ++i)
            cout << ' ';
        cout << ']';

        cout << flush;

        next_tick += 1;
    }
    cout_lock.unlock();
    return true;
}
// AUTHOR: Victor Alessandrini, 2015
// VATH libray,in book "Shared Memory Application
// Programming"
// ==============================================
// SpinLock.h
// 
// This is a custom spin mutex, using std::atomic
// This implementation uses the stronger seq_cst
// memory model because GNU 4.8 does not accept
// other options. With other compilers, try switching
// the commented lines in the code below. 
// --------------------------------------------

#ifndef  SPIN_LOCK_H
#define  SPIN_LOCK_H

#include <atomic>

class SpinLock
    {
    private:
     std::atomic<bool> state;

    public:
    SpinLock() 
       {
       state.store(false, std::memory_order_seq_cst);
       //state.store(false, std::memory_order_acquire);
       }

    void Lock()
       {
       while (state.exchange(true, std::memory_order_seq_cst) == true);
       //while (state.exchange(true, std::memory_order_acquire) == true);
       }
	    
    void Unlock()
       {
       state.store(false, std::memory_order_seq_cst);
       //state.store(false, std::memory_order_release);
       }
    };

#endif  /* SPIN_LOCK_H */

// AUTHOR: Victor Alessandrini, 2015
// VATH libray,in book "Shared Memory Application
// Programming"
// ***********************************************
// BLock.h
//
// Boolean Lock interface, used to encapsulate the wait - notify
// mechanism between threads.
// This is a rather complete implementation, with timed waits
// on demand. The waiting thread will be released when the waiting
// condition is signaled or when the waiting time interval expires.
// =============================================================

#ifndef BLOCK_H
#define BLOCK_H

#include <mutex>
#include <condition_variable>

class BLock
    {
    private:

    bool state;
    std::mutex                 bmutex;   // protects state
    std::condition_variable    bcond;    // condition variable         

    public:
	
    BLock();
    BLock(bool B );
    ~BLock();

    void  SetState( bool value );
    bool  GetState();

    int  Wait_Until(bool st, long timeout);

    void Set_And_Notify(bool st);
    void Set_And_Notify_All(bool st);
    };

#endif

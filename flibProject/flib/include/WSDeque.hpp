// AUTHOR: Victor Alessandrini, 2016
// VATH libray,in book "Shared Memory Application
// Programming"
// ***********************************************
//  WSDeque.h
//
//  Thread safe Deque class, implementing work stealing. 
//  Algorithm is taken from "The art of multiprocessor
//  programming", page 387.
//
//  This class encapsulates the std::deque class, adding
//  the necessary functionality (mutex locking) to
//  enforce thread safety. This queue operates in the
//  following way:
//
//  The queue is "owned" by one specific thread in a 
//  pool. Each worker thread has its own queue.
//
//  The owner queue inserts tasks by calling push().
//  No other thread can insert tasks in the queue.
//
//  The owner thread retrieves the most recently
//  queued task from the bottom of the queue, by
//  calling try_pop().
//
//  Other threads steal the oldest tasks in the queue 
//  by popping from the top, calling try_steal().
//
//  Notice that, in the absence of task stealing,
//  the queue is accessed only by the owner thread,
//  and would naturally be thread safe.
// *************************************************

#ifndef WS_DEQUE
#define WS_DEQUE

#include <mutex>
#include <atomic>
#include <condition_variable>
#include <iostream>
#include <deque>
#include <Task.hpp>

class WSDeque
   {
   private:
   ///////
    std::deque<Task*> the_queue;
    mutable std::mutex the_mutex;

   public:
   //////

    WSDeque() { }                                // default constructor
    WSDeque(const WSDeque& other) = delete;              // copy deleted
    WSDeque& operator=(const WSDeque& other) = delete;   // assign deleted

    bool isEmpty()
       { return the_queue.empty(); }

    void push(Task *t)
       {
       std::lock_guard<std::mutex> lock(the_mutex);
       the_queue.push_front(t);
       }

   Task* try_pop()
       {
       std::lock_guard<std::mutex> lock(the_mutex);
       if(the_queue.empty()) return NULL;
       Task *t = the_queue.front();
       the_queue.pop_front();
       return t;
       }
   
   Task* try_steal()
       {
       std::lock_guard<std::mutex> lock(the_mutex);
       if(the_queue.empty()) return NULL;
       Task *t = the_queue.back();
       the_queue.pop_back();
       return t;
       }
    };

#endif // WS_DEQUE 

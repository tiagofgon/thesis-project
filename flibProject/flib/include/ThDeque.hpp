// AUTHOR: Victor Alessandrini, 2015
// VATH libray,in book "Shared Memory Application
// Programming"
// ***********************************************
/*  ThDeque.h
 *
 *  Safe deque class, adapted to the needs of the thread pool 
 *  utility. We will use this utility to queue tasks and
 *  jobs (collections of tasks). 
 *
 *  When the queue is empty, threads go to sleep on a CV. 
 *
 *  We do not need a capacity for the usage of this class, and
 *  the Add operation never blocks.
 *
 *  Remove is called by idle threads, and operates like the
 *  same function in the ThQueue class. TryRemove is called by
 *  threads that have suspeded a task, and removes items from
 *  the back. 
 * ************************************************************/

#ifndef TH_DEQUE
#define TH_DEQUE

#include <deque>
#include <mutex>
#include <condition_variable>
#include <iostream>

using namespace std;

template <class T>
class ThDeque
    {
    protected:

    std::deque<T> c;          // container for the elements
    bool active;
    std::mutex qlock;
    std::condition_variable  not_empty;
    
    public:

    int size;

    // ************
    // Constructor
    // ************

    ThDeque()   
        {
        size = 0;
        active = true;
        }


    // **********
    // Destructor
    // **********

     ~ThDeque(){}    // destructor

    int GetSize()
        {
        std::lock_guard<std::mutex> lock(qlock);
        int ret = size;        
        return ret;
        }

    // ******************************      
    // insert element into the queue
    // ******************************

    void Add (const T& elem) 
        {
        std::unique_lock<std::mutex> lock(qlock);
        // .....................................................
        c.push_back(elem);
        size++;

        // If queue was empty, signal add event
        // ------------------------------------ 
        if(size==1)
            not_empty.notify_all();
        // ....................................................
        }

    // ***************************************************
    // Removes element from the queue and return its value
    // The boolean flag indicates if the return value T
    // is valid. 
    // If remove with an inactive empty queue, it does
    // not waits, it returns an invalid value.
    // ***************************************************
    T Remove(bool& flag) 
        {
        std::unique_lock<std::mutex> *lock;
        lock = new std::unique_lock<std::mutex>(qlock);

        // wait if queue is empty and active
        // ---------------------------------
        while(size==0 && active==true ) 
	     not_empty.wait(*lock);
       
       /* -----------------------------------------------
        * I reached here for several reasons
        * -) size=0, inactive : return a false value
        * -) size >0, active or inactive : pop and return,
        *    signaling "queue not full if appropriate
        * -----------------------------------------------*/

       if(size == 0 && active==false)
           {
           // .................................
           delete lock;
           T fake;
           flag = false;
         
           // -----------------------------------------------------
           // Before returning, remember that other threads may be
           // waiting for "queue not empty", signal the condition
           // so that they may also wake up and return a fake value
           // -----------------------------------------------------
           not_empty.notify_all();
           return fake;
           }
        
        // ----------------------------------------------------
        // Here we are sure that thread woke up by genuine post
        // Remove and return
        // ----------------------------------------------------
        T elem(c.front());
        c.pop_front();
        size--;
        // ..................................
        delete lock;
        flag = true;
        return elem;
        }


    // ***************************************************
    // Tries to remove head element from the deque, but 
    // does not wait if the deque is empty
    // The boolean flag is an output parameter. It indicates 
    // if the return value T is a valid element. 
    // ***************************************************
    T TryRemoveFront(bool& flag) 
        {
        std::unique_lock<std::mutex> *lock;
        lock = new std::unique_lock<std::mutex>(qlock);

       /* -----------------------------------------------
        * Strategy
        * If size==0, returns a fake value. Otherwise, returns
        * a valid value.
        * -----------------------------------------------*/

       if(size == 0)
           {
           T fake;
           flag = false;
           delete lock;
           return fake;
           }
       else
           { 
           // ---------------------------------------------
           // Here we are sure that there is a head element
           // ---------------------------------------------
           T elem(c.front());
           flag = true;
           c.pop_front();
           size--; 
           delete lock;
           return elem;
           }
       }


    // **************************************************
    // Same function as before, nut now the remove takes
    // place at the tail of the deque.
    // ************************************************** 
    T TryRemoveBack(bool& flag) 
        {
        std::unique_lock<std::mutex> *lock;
        lock = new std::unique_lock<std::mutex>(qlock);

       /* -----------------------------------------------
        * Strategy
        * If size==0, returns a fake value. Otherwise, returns
        * a valid value.
        * -----------------------------------------------*/

       if(size == 0)
           {
           T fake;
           flag = false;
           delete lock;
           return fake;
           }
       else
           { 
           // ---------------------------------------------
           // Here we are sure that there is a head element
           // ---------------------------------------------
           T elem(c.back());
           flag = true;
           c.pop_back();
           size--;
           delete lock; 
           return elem;
           }
       }

   // **************************************
   // Close queue. The only effect of this
   // call is to stop all Add() actions from
   // producer threads
   // **************************************
   void CloseQueue()
       {
        std::unique_lock<std::mutex> lock(qlock);
        active = false;   // queue is closed

        // ---------------------------------------------------------
        // From now on, no more adds are possible. But, notice that
        // it could happen that there are consumer threads waiting
        // for "non-empty" condition in a remove operation. Wake
        // them up so that they finish properly (nobody else will
        // do that because there are no more adds)
        // ---------------------------------------------------------
        if(size==0) not_empty.notify_all();
        }
   };

#endif // TH_DEQUE 

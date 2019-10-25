// AUTHOR: Victor Alessandrini, 2015
// VATH libray,in book "Shared Memory Application
// Programming"
// ***********************************************
/*  ThQueue.h
 *
 *  Safer queue class. When the queue is *  empty, threads go 
 *  to sleep on a CV. 
 *  The STL is never full, but we define a capacity because it
 *  is useful to control permanent producers
 *  
 * Instances like ThQueue<double>, ThQueue<int>, and so on can
 * be used directly to build pipes between threads.
 *
 * This tool is also used for thread pools, to pipeline work
 * requests.
 *
 * NOTE: New version, adding Peek() and TryRemove() member
 * functions.
 * ************************************************************/

#ifndef THQUEUE
#define THQUEUE

#include <queue>
#include <mutex>
#include <condition_variable>
#include <iostream>

using namespace std;

template <class T>
class ThQueue 
    {
    protected:

    std::queue<T> c;        // container for the elements
    int           capacity;
    bool          active;

    std::mutex               qlock;
    std::condition_variable  not_empty;
    std::condition_variable  not_full;
    
    public:

    int size;

    // -----------
    // Constructor
    // -----------
    ThQueue(int cap)   
        {
        capacity = cap;
        active = true;
        size = 0;
        }


    // ----------
    // Destructor
    // ----------
     ~ThQueue(){}    // destructor
 
 
    int GetSize()
        {
        std::lock_guard<std::mutex> lock(qlock);
        int ret = size;        
        return ret;
        }

    // ------------------------------------------
    // insert element into the queue, but only if
    // the queue is active. Return 0 if the add
    // failed because the queue is inactive.
    // ------------------------------------------
    int Add (const T& elem) 
        {
        int retval, cancel, tmp;
        
        std::unique_lock<std::mutex> lock(qlock);
        // ......................................................
        while(size==capacity)                    // wait if full
            not_full.wait(lock);

        // At this point, queue is not full. 
        // ---------------------------------
        if(active == true)           // check if queue is active
            {
            c.push(elem);
            size++;
            tmp = 1;

            // If queue was empty, signal add event
            // ------------------------------------ 
            if(size==1) not_empty.notify_all();
             }
        else tmp = 0;
        return tmp;
        // ....................................................
        }


    // ---------------------------------------------------
    // Removes element from the queue and return its value
    // The boolean flag indicates if the return value T
    // is valid. 
    // If remove with an inactive empty queue, it does
    // not wait, it returns an invalid value.
    // ---------------------------------------------------
    T Remove(bool& flag) 
        {
        int retval, cancel, tmp;
        std::unique_lock<std::mutex> *lock;
        lock = new std::unique_lock<std::mutex>(qlock);
       
        // wait if queue is empty and active
        // ---------------------------------
        while(size==0 && active==true ) 
	     not_empty.wait(*lock);
       
       // -----------------------------------------------
       // I reached here for several reasons
       // -) size=0, inactive : return a false value
       // -) size >0, active or inactive : pop and return,
       //    signaling "queue not full if appropriate
       // -----------------------------------------------
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
        c.pop();
        size--;

        // If queue was full, signal the remove event
        // ------------------------------------------
        if(size==(capacity-1))
            not_full.notify_all();
        delete lock;
        flag = true;
        return elem;
        }

    // ***************************************************
    // NEW CODE:
    // --------
    // Tries to remove head element from the queue, but does
    // not wait if the queue is empty
    // The boolean flag indicates if the return value T
    // is valid. 
    // ***************************************************

    T TryRemove(bool& flag) 
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
           c.pop();
           size--; 
           delete lock;
           return elem;
           }
       }

    // ***************************************************
    // Reads element from the queue and return its value
    // The boolean flag indicates if the return value T
    // is valid. 
    // If reads with an inactive empty queue, it does
    // not wait, it returns an invalid value.
    // ***************************************************

    T Read(bool& flag) 
        {
        int retval, cancel, tmp;;

        std::unique_lock<std::mutex> *lock;
        lock = new std::unique_lock<std::mutex>(qlock);

        // wait if queue is empty and active
        while(size==0 && active==true ) 
	  not_empty.wait(*lock);
       
       /* -----------------------------------------------
        * I reached here for several reasons
        * -) size=0, inactive : return a false value
        * -) size >0, active or inactive : return,
        *    DO NOT SIGNAL "queue not full" if appropriate
        * -----------------------------------------------*/

       if(size == 0 && active==false)
           {
           delete lock;
           T fake;
           flag = false;
           return fake;
           }
        
        // ----------------------------------------------------
        // Here we are sure that thread woke up by genuine post
        // Remove and return
        // ----------------------------------------------------
        T elem(c.front());
        delete lock; 
        flag = true;
        return elem;
        }

   // **************************************
   // Close queue. The only effect of this
   // call is to stop all Add() actions from
   // producer threads
   // **************************************
   
   void CloseQueue()
       {
        int retval;
        std::unique_lock<std::mutex> lock(qlock);
        active = false;   // queue is closed

        // ---------------------------------------------------------
        // From now on, no more adds are possible. But, notice that
        // it could happen that there are consumer threads waiting
        // for "non-empty" condition in a remove operation. Wake
        // them up so that they finish properly (nobody else will
        // do that because there are no more adds)
        // ---------------------------------------------------------
        if(size==0)
            not_empty.notify_all();
        }

   // ***********************************
   // Drain the queue. called by consumer
   // threads, after closing the queue
   // ***********************************

   void DrainQueue()
      {
      T data;
      bool state;
      do
         {
         data = Remove(state);
         }while(state);
      }

   };

#endif /* THQUEUE */

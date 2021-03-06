// AUTHOR: Victor Alessandrini, 2015
// VATH libray,in book "Shared Memory Application
// Programming"
// ====================================================
// TBarrier.h
//
// ------------------------------------------------------
// A barrier implementaion, based on the SenseBarrier
// example in "The Art of Multiprocessor Programming", 
// page 399
// ------------------------------------------------------
//
// Barrier algorithm using TBB atomic and thread_local
// features. This is the TBB version of the ABarrier
// class, using similar features in C++11 or Windows.
// This class may be useful to replace ABarrier in a
// Pthreads environment (there are no atomic utilities
// in Pthreads).
// ----------------------------------

#ifndef  TBB_BARRIER_H
#define  TBB_BARRIER_H
#include <tbb/atomic.h>   
#include <tbb/enumerable_thread_specific.h>    

class TBarrier
   {
   public:
     tbb::atomic<int> count;
     tbb::atomic<bool> sense;
     int size;
     tbb::enumerable_thread_specific<bool> ThreadSense;
     
     TBarrier(int n)
       {
       count = n;
       size = n;
       sense = false;
       tbb::enumerable_thread_specific<bool> seed(!sense);
       }

    void Wait()
       {
       bool current_sense;
       tbb::enumerable_thread_specific<bool>::reference mySense
            = ThreadSense.local();
       int position = count.fetch_and_add(-1);  // decreases count and 
                                                // returns old value
       if(position==1)
          {
          count.fetch_and_store(size);
          sense.fetch_and_store(mySense);
          }
       else
          { 
          do
             {
             current_sense = sense;
             }while( current_sense != mySense); 
          }
       mySense = !mySense;
       }
   };
   
#endif  /* TBB_BARRIER_H */

// ***********************************************
// AUTHOR: Victor Alessandrini, 2015
// VATH libray,in book "Shared Memory Application
// Programming"
// ***********************************************
// ABarrier.h
// 
// A barrier implementation, based on the SenseBarrier
// example in "The Art of Multiprocessor Programming", 
// page 399
// 
// Using std::atomic and the thread_local service in
// C++11
// -------------------------------------------------

#ifndef  A_BARRIER_H
#define  A_BARRIER_H
#include <atomic>   

class ABarrier
   {
   public:
     std::atomic<int> count;
     std::atomic<bool> sense;
     int size;
     
     ABarrier(int n)
       {
       count = n;
       size = n;
       sense = false;
       }

    void Wait()
       {
       thread_local bool mySense = !sense;   // initialization
       bool current_sense;
       int position = count.fetch_sub(1);  // decreases count and 
                                           // returns old value
       if(position==1)
          {
          count.exchange(size);
          sense.exchange(mySense);
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
   
#endif  /* A_BARRIER_H */

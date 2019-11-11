// ***********************************************
// AUTHOR: Victor Alessandrini, 2015
// VATH libray,in book "Shared Memory Application
// Programming"
// ***********************************************
// Barrier.h
// This class implements a traditional barrier
// algorithm, using mutexes and condition variables. 
// ----------------------------------------------

#ifndef  BARRIER_H
#define  BARRIER_H

#include <mutex>
#include <condition_variable>

class  Barrier
   {
   private:
     std::mutex               count_mutex;         
     std::condition_variable  count_cond;     // condition variable         
     int                      Nth;            // number of threads
     int                      count;          // counts active threads      
     bool                     predicate;      // toggles between calls
 
   public:
     Barrier(int nth);
     ~Barrier();
     int Wait();
   };

#endif  /* BARRIER_H */

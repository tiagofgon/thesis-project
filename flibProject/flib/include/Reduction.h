// AUTHOR: Victor Alessandrini, 2015
// VATH libray,in book "Shared Memory Application
// Programming"
// ***********************************************
// Reduction.h.
//
// Template class to perform a (sum) reduction of a
// data type T in a thread pool environment.
//
// Just encapsulates a mutex protected type T data 
// item holding the reduction result
//
// Can be easily modified to implement other reduction
// operations
// ------------------------------------------------------
#ifndef REDUCTION_H
#define REDUCTION_H

#include <mutex>

template<typename T>
class Reduction
   {
   private:
   T data;
   std::mutex rmutex; 

   public:
   Reduction() {}

   void Accumulate(T input) 
      {
      std::lock_guard<std::mutex> lock(rmutex);
      data += input; 
      }
   
   T Data()
      {
      T retval;
      std::lock_guard<std::mutex> lock(rmutex);
      retval = data;
      return retval;
      }

   void Reset()
      {
      data -= data;
      }
   }; 

#endif

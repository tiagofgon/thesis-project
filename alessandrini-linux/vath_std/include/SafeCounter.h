// AUTHOR: Victor Alessandrini, 2015
// VATH libray,in book "Shared Memory Application
// Programming"
// ***********************************************
// SafeCounter.h.
//
// Usage: SafeCounter SC(N) : 
//        Successive calls to SC.Next() return N+1, N+1...
//        If N is not specified, default is 0
//
// Useful when different threads or tasks need to be
// identified with a unique rank index.
// ----------------------------------------------------
#ifndef SAFECOUNTER_H
#define SAFECOUNTER_H

#include <mutex>

class SafeCounter
   {
   private:
   int data;
   std::mutex smutex; 

   public:
   SafeCounter() : data(0) {}

   ~SafeCounter() { }

   int Next() 
      {
      int retval;
      std::lock_guard<std::mutex> lock(smutex);
      data ++;
      retval = data; 
      return retval; 
      }
   
   void Reset()
      {
      std::lock_guard<std::mutex> lock(smutex);
      data = 0;
      }
   }; 

#endif

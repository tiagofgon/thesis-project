// **************************************
// Copyright (c) 2012 Victor Alessandrini
// All rights reserved.
// **************************************
// File UpdateTbb.h
//
// Using a lock free algorithm to accumulate
// T values, using the atomic<T> TBB class
//
// ----------------------------------
#include <tbb/atomic.h>

template<typename T>
class UpdateTbb
   {
   private:
    tbb::atomic<T*> P;

   public:
    UpdateTbb() { P = new T(); }
    ~UpdateTbb() { delete P; }

    T Data() { return *P; }
    void Reset() 
       { 
       T value  = *P;
       *P -= value;
       }

    void Update(T d)
       {
       T *oldptr, *newptr;
       do
         {
         oldptr = P;
         newptr = new T(*P+d);
         }while(P.compare_and_swap(newptr, oldptr) != oldptr);
       }
   };

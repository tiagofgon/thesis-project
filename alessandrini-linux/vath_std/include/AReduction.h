// ***********************************************
// AUTHOR: Victor Alessandrini, 2015
// VATH libray,in book "Shared Memory Application
// Programming"
// ***********************************************
// File AReduction.h
//
// Atomic Pointer update, STD C++11
// Using a lock free algorithm to accumulate
// T values, using the std::atomic<T*> class
// -----------------------------------------------
#include <atomic>

template<typename T>
class AReduction
   {
   private:
    std::atomic<T*> P;

   public:
    AReduction() { P = new T(); }

	~AReduction() { delete P; }

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
         }while(P.compare_exchange_strong(oldptr, newptr)==false);
       }
   };

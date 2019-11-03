// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// Cpp5_S.C
//
// This program launches a worker thread that performs
// a computation.
//
// Same as example 3:  we launch a thread by passing a
// function object to the thread constructor. But now,
// the function object is modified in such a way that
// the main thread can read the square root value. 
//
// A new field , "double result", is added to the
// function object, where the result of the computation
// is stored. The field is public, so that it can be
// read after te computation is done.
//
// As discussed in the book, for this to work the
// thread function must get a reference to the function
// object.
// -------------------------------------------------

#include <iostream>
#include <thread>
#include <chrono>
#include <math.h>

class Worker
   {
   private:
    unsigned Niters;

   public:
    double result;

    Worker(unsigned N) : Niters(N) {}

    void operator()(double target, double guess)
       {
       std::cout << "Worker: computing sqrt(" << target
                 << "), iterations = " << Niters << std::endl;

       // use Newton's method
       // -------------------
       double x;
       double xl = guess;
       for(unsigned i=0; i<Niters; i++)
          {
          x = xl - (xl*xl - target) / (2 * xl);
          if(fabs(x-xl) < 0.0000001) break;
          xl = x;
          std::cout << "Iter " << i << " = " << x << std::endl;
          }
       result = x;
       }
   };


int main(int argc, char* argv[])
   {
   std::cout << "main: startup" << std::endl;

   Worker w(500);              // object w is constructed
   std::thread T(std::ref(w), 25, 7);  // thread function is w(25, 4)

   T.join();

   // Now, main can read the result stored in w
   // -----------------------------------------
   std::cout << "main: result obtained is " << w.result << std::endl;
   return 0;
   }



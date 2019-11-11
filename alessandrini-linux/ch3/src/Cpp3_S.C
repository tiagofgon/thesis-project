// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// Cpp3_S.C : third C++11 example
//
// This program launches a worker thread that performs
// a computation: computing the square root of a given
// number, using the Newton method.
//
// The new thing in this example is that the thread is
// launched by passing a function object to the thread 
// constructor, rather than an ordinary function
// -------------------------------------------------

#include <iostream>
#include <thread>
#include <chrono>
#include <math.h>

// ------------------------------------------------------
// This is the function object class. The new thread must 
// get three input items to perform its assignment:
//
// -) the target value whose square root is computed
// -) an initial guess to start the iterative procedure
// -) the maximum number of iterations tolerated
//
// To show the flexibility of function objects, we choose to
// pass the first two items as arguments to the function executed 
// by the function object. The third item is passed via the 
// function object constructor.
// ----------------------------------------------------------
class Worker
   {
   private:
    unsigned Niters;

   public:
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
       std::cout << "Worker: answer = " << x << std::endl;
       }
   };

// ----------------------------------------------------------------
// Notice that in this example we do not return to main() the result
// of the computation. The square root is printed to stdout by the
// worker thread that computed it. The next example 5 proposes a
// way of returning the square root value to the main thread
// -----------------------------------------------------------------
int main(int argc, char* argv[])
   {
   std::cout << "main: startup" << std::endl;

   Worker w(612);            // the object w is constructed
   std::thread T(w, 25, 4);  // the thread function is w(25, 4)

   std::cout << "main: waiting for thread " << std::endl;
   T.join();
   std::cout << "main: done" << std::endl;
   return 0;
   }



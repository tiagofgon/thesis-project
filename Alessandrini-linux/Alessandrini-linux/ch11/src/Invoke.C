// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File Invoke.C
// -----------------------------------
// MonteCarlo computation of PI using
// TBB threads and parallel_invoke
// 
// In this example, parallel_invoke is used to implement 
// the parallel execution of two function objects (instances
// of the same functor class). This is adequate when arguments 
// must be passed to the concurrent functions.
// ----------------------------------------------------------

#include <iostream>
#include <stdlib.h>
#include <Rand.h>
#include <SafeCounter.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/parallel_invoke.h>
#include <tbb/tick_count.h>

using namespace tbb;

// Global variables
// ----------------
int  nTh;
long nsamples;

// These two variables are introduced to receive return values
// (acceptances) from each one of the function objects executed
// in parallel. They are passed by reference to the function
// objects.
// ------------------
long count1, count2;

// This is an ordinary function that performs a MC run, and
// returns the acceptance to an external variable passed by
// reference
// --------------------------------------------------------
void my_function(int n, long& C)
   {
   double x, y;
   Rand R(999*n);

   long count = 0;
   for(size_t n=0; n<nsamples; n++)
      {
      x = R.draw();
      y = R.draw();
      if((x*x+y*y) <= 1.0 ) count++;
      }
   C = count;
   }

// ------------------------------------------------------
// This is a class that basically transforms the ordinary
// function my_function into a function object. Lambda
// expressions could have been used to do the same thing
// ------------------------------------------------------
class Acceptance
   {
   private:
    int rank;
    long& L;

   public:
    Acceptance(int n, long& l) : rank(n), L(l) {}

    void operator()() const
       { my_function(rank, L); }
   }; 
       

// -----------------        
// The main function
// -----------------
int main(int argc, char **argv)
   {
   if(argc==2) nTh = atoi(argv[1]);
   else nTh = 2;
   nsamples = 100000000;

   task_scheduler_init init(nTh);

   // Next, two function objects f1 and f2 are defined
   // The first argument defines a rank ID for tasks
   // ------------------------------------------------
   Acceptance f1(1, count1);
   Acceptance f2(2, count2);

   tbb::tick_count t0 = tbb::tick_count::now();
   // --------------------------------------------------
   parallel_invoke(f1, f2);
   std::cout << count1 << "   " << count2 << std::endl;
   double pi = 2.0 * (double)(count1+count2) / nsamples;
   std::cout << "\n Value of PI = " << pi << std::endl;
   // ---------------------------------------------------
   tbb::tick_count t1 = tbb::tick_count::now();
   std::cout << "\n Wall time = " << (t1-t0).seconds() << " seconds"
             << std::endl;
   }

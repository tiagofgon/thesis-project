// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File InvokeFct.C
// -----------------------------------
// MonteCarlo computation of PI
// TBB threads, using parallel_invoke.
//
// In this example, parallel_invoke is used to implement 
// the parallel execution of two ordinary functions. This 
// is fine when the functions receive no arguments
// -----------------------------------------------------

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
long count1, count2;


// This function operates on count1
// --------------------------------
void f1()
   {
   double x, y;
   Rand R(999);

   count1 = 0;
   for(size_t n=0; n<nsamples; n++)
      {
      x = R.draw();
      y = R.draw();
      if((x*x+y*y) <= 1.0 ) count1++;
      }
   }

// This function operates on count 2
// ---------------------------------
void f2()
   {
   double x, y;
   Rand R(999*2);

   count2 = 0;
   for(size_t n=0; n<nsamples; n++)
      {
      x = R.draw();
      y = R.draw();
      if((x*x+y*y) <= 1.0 ) count2++;
      }
   }

        
// The main function
// -----------------

int main(int argc, char **argv)
   {
   SafeCounter SC;

   if(argc==2) nTh = atoi(argv[1]);
   else nTh = 2;
   nsamples = 100000000;

   task_scheduler_init init(nTh);

   tbb::tick_count t0 = tbb::tick_count::now();
   // -------------------------------------------------
   parallel_invoke(f1, f2);
   std::cout << count1 << "   " << count2 << std::endl;
   double pi = 2.0 * (double)(count1+count2) / nsamples;
   std::cout << "\n Value of PI = " << pi << std::endl;
   // -------------------------------------------------
   tbb::tick_count t1 = tbb::tick_count::now();
   std::cout << "\n Wall time = " << (t1-t0).seconds() 
             << " seconds" << std::endl;
   }

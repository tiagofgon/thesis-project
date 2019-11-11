// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File ScaProd_T.C
//
// The TbbReduction class is a mofidied version of the
// Reduction class: instead of mutex locking, it uses 
// a lock free algorithm to accumulate double values 
// in a scalar product calculation.
//  
// TbbReduction is based on tbb::atomic<T> class.
// This example is portable code.
//
// Again, this example has excessive mutual exclusion to
// look at the performance of the lock free algorithms. 
//
// Multithreaded code (2 threads)
// ----------------------------------------------------
#include <stdlib.h>
#include <CpuTimer.h>
#include <iostream>
#include <Rand.h>
#include <SPool.h>             
#include <TbbReduction.h>

#define VECSIZE  10000000

// --------------------------------------
// A class for a thread safe non blocking
// update operation of a double data item,
// using the compare_and_swap function.
// --------------------------------------
using namespace std;

// Global variables
// ----------------
SPool TH(2);              // set of two threads
long nsamples;            // number of MonteCarlo events per thread
Rand R(888);              // random number generator
TbbReduction<double> D;   // lock free accumulator of doubles 

double A[VECSIZE];
double B[VECSIZE];

void thread_fct(void *P)
   {
   double d;
   int beg, end;
   beg = 0;                    // initialize [beg, end) to global range
   end = VECSIZE;
   TH.ThreadRange(beg, end);   // [beg, end) is now range for this thread
   for(int n=beg; n<end; n++)
      {
      d = A[n]*B[n];
      D.Update(d);
      }
   }

int main(int argc, char **argv)
   {
   CpuTimer TR;      // object to measure execution times
   Rand R(999);      // random generator used to initialize vectors
   
   for(int n=0; n<VECSIZE; n++)
      {
      A[n] = -1.0 + 2.0 * R.draw();      // value in [-1, 1]
      B[n] = -1.0 + 2.0 * R.draw();      // value in [-1, 1]
      }
   
   TR.Start();
   TH.Dispatch(thread_fct, NULL);
   TH.WaitForIdle();
   TR.Stop();

   cout << "\n Scalar pruduct is = " << D.Data() << endl;
   TR.Report();
   }

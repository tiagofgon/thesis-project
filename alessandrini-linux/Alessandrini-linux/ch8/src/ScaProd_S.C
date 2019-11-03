// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File ScaProd_S.C
//
// The AReduction class is a different version of the
// Reduction class: instead of mutex locking, it uses 
// a lock free algorithm to accumulate double values 
// in a scalar product calculation.
//  
// AReduction is based on std::atomic<T> class.
// This example is pure C++11 code.
//
// Again, this example has excessive mutual exclusion to
// assess the performance of the lock free algorithms. 
//
// Multithreaded code (2 threads)
// ----------------------------------
#include <stdlib.h>
#include <CpuTimer.h>
#include <iostream>
#include <Rand.h>
#include <SPool.h>                         
#include <AReduction.h>                         

#define VECSIZE  10000000

// Global variables
// ----------------
SPool *TH;                // set of two threads
Rand R(888);              // random number generator
AReduction<double> D;     // lock free accumulator of doubles 

double A[VECSIZE];
double B[VECSIZE];

void thread_fct(void *P)
   {
   double d;
   int rank = TH->GetRank();
   int beg, end;
   beg = 0;                    // initialize [beg, end) to global range
   end = VECSIZE;
   TH->ThreadRange(beg, end);   // [beg, end) is now range for this thread
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
   TH = new SPool(2);

   for(int n=0; n<VECSIZE; n++)
      {
      A[n] = -1.0 + 2.0 * R.draw();      // value in [-1, 1]
      B[n] = -1.0 + 2.0 * R.draw();      // value in [-1, 1]
      }
   
   TR.Start();
   TH->Dispatch(thread_fct, NULL);
   TH->WaitForIdle();
   TR.Stop();

   std::cout << "\n Scalar product is = " << D.Data() << std::endl;
   TR.Report();
   delete TH;
   }

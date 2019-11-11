// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// **************************************
// File ScaProd4.C
// 
// Scalar product of two vectors. Same as ScaProd3_P.C (mutex 
// is locked only once per thread), but now we use the Reduction 
// utility class that encapsulates the variable that accumulates 
// results as well as the protecting mutex.
// --------------------------------------------------------------
#include <stdlib.h>
#include <CpuTimer.h>
#include <Rand.h>
#include <SPool.h>
#include <Reduction.h>
#include <iostream>
#define VECSIZE  10000000

using namespace std;

// Global variables
// ----------------
double A[VECSIZE];
double B[VECSIZE];

Reduction<double> RD;   // the accumulator of doubles
SPool TH(2);            // two worker threads

void thread_fct(void *P)
   {
   double prod;
   int beg, end;
   beg = 0;                    // initialize [beg, end) to global range
   end = VECSIZE;
   TH.ThreadRange(beg, end);   // now [beg, end) is the range for this
                               // thread
   prod = 0.0;
   for(int n=beg; n<end; n++) prod += A[n]*B[n];
   RD.Accumulate(prod);
   }


int main(int argc, char **argv)
   {
   CpuTimer TR;       // object to measure execution times
   Rand R(999);       // random generator used to initialize vectors
   
   for(int n=0; n<VECSIZE; n++)
      {
      A[n] = -1.0 + 2.0 * R.draw();      // value in [-1, 1]
      B[n] = -1.0 + 2.0 * R.draw();      // value in [-1, 1]
      }
   
   TR.Start();
   for(int n=0; n<6; n++)      // perform the same job 6 times
      {
      RD.Reset();
      TH.Dispatch(thread_fct, NULL);
      TH.WaitForIdle();
      }
   TR.Stop();

   cout << "\n Scalar product is = " << RD.Data() << endl;
   TR.Report();
   cout << "\n Result obtained using the Reduction class\n" << endl;
   }

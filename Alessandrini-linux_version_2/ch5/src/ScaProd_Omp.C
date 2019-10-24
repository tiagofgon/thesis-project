// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// **************************************
// File ScaProd_Omp.C
// 
// Scalar product of two vectors
//
// In this example, we are using a OpenMP "omp_lock" mutex 
// to accumulate partial results. 
//
// The mutex is locked every time a new partial result is 
// computed, and there is excessive mutex contention.
// ------------------------------------------------------
#include <stdlib.h>
#include <CpuTimer.h>
#include <Rand.h>
#include <omp.h>
#include <ThreadRangeOmp.h>
#include <iostream>

#define VECSIZE  10000000

using namespace std;

// Global variables
// ----------------
double A[VECSIZE];
double B[VECSIZE];
double scalarprod;     

omp_lock_t mylock;

void omp_task()
   {
   double d;
   int beg, end;
   beg = 0;                    // initialize [beg, end) to global range
   end = VECSIZE;
   ThreadRangeOmp(beg, end);   // now [beg, end) is the range for this
                               // thread
   for(int n=beg; n<end; n++)
      {
      d = A[n]*B[n];
      omp_set_lock(&mylock);
      scalarprod += d;
      omp_unset_lock(&mylock);
      }
   }

int main(int argc, char **argv)
   {
   CpuTimer TR;         // object to measure execution times
   Rand R(999);         // random generator used to initialize vectors
   omp_init_lock(&mylock);
   
   scalarprod = 0.0;
   for(int n=0; n<VECSIZE; n++)
      {
      A[n] = -1.0 + 2.0 * R.draw();      // value in [-1, 1]
      B[n] = -1.0 + 2.0 * R.draw();      // value in [-1, 1]
      }
   
   TR.Start();
   #pragma omp parallel num_threads(2)
       {
       omp_task();
       }
   TR.Stop();

   cout << "\n Scalar pruduct is = " << scalarprod << endl;
   TR.Report();
   omp_destroy_lock(&mylock);
   }

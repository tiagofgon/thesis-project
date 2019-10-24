// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// **************************************
// File ScaProd2_P.C
// 
// Scalar product of two vectors
//
// In this example, we are using a Pthreads "spinlock mutex" to 
// accumulate partial results. 
//
// The mutex is locked every time a new partial result is 
// computed, and there is excessive mutex contention. It
// is observed that the spin mutex has nevertheless better
// performance than the ordinary mutex.
// ------------------------------------------------------
#include <stdlib.h>
#include <CpuTimer.h>
#include <Rand.h>
#include <SPool.h>
#include <pthread.h>
#include <iostream>

#define VECSIZE  10000000

using namespace std;

// Global variables
// ----------------
double A[VECSIZE];
double B[VECSIZE];
double scalarprod;     
pthread_spinlock_t mylock;

SPool TH(2);     // two worker threads

void thread_fct(void *P)
   {
   double d;
   int beg, end;
   beg = 0;                    // initialize [beg, end) to global range
   end = VECSIZE;
   TH.ThreadRange(beg, end);   // now [beg, end) is the range for this
                               // thread
   for(int n=beg; n<end; n++)
      {
      d = A[n]*B[n];
      pthread_spin_lock(&mylock);
      scalarprod += d;
      pthread_spin_unlock(&mylock);
      }
   }

int main(int argc, char **argv)
   {
   CpuTimer TR;         // object to measure execution times
   Rand R(999);         // random generator used to initialize vectors
   pthread_spin_init(&mylock, PTHREAD_PROCESS_PRIVATE);
   
   for(int n=0; n<VECSIZE; n++)
      {
      A[n] = -1.0 + 2.0 * R.draw();      // value in [-1, 1]
      B[n] = -1.0 + 2.0 * R.draw();      // value in [-1, 1]
      }
   
   TR.Start();
   for(int n=0; n<6; ++n)   // perform the same job 6 times
      {
      scalarprod = 0.0;
      TH.Dispatch(thread_fct, NULL);
      TH.WaitForIdle();
      }
   TR.Stop();

   cout << "\n Scalar pruduct is = " << scalarprod << endl;
   TR.Report();
   cout << "\n Reasonable scaling in spite of large synchro overhead ";
   cout << "\n No system time with the spinlock mutex\n"
        << endl;
   pthread_spin_destroy(&mylock);
   }

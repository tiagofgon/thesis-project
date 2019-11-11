// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// **************************************
// File ScaProd3_P.C
// 
// Scalar product of two vectors. 
// The correct way: results are accumulated in a local 
// variable. Each thread collects its contribution to the
// scalar product. Then, results from threads are finally
// accumulated in a global variable, but the mutex is locked 
// only once for each thread.
// ----------------------------------------------------
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
pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;

SPool TH(2);     // two worker threads

void thread_fct(void *P)
   {
   double prod;    // stores partial results from this thread
   int beg, end;
   beg = 0;                   // initialize [beg, end) to global range
   end = VECSIZE;
   TH.ThreadRange(beg, end);  // now [beg, end) is the range for this
                              // thread
   prod = 0.0;
   for(int n=beg; n<end; n++) prod += A[n]*B[n];

   pthread_mutex_lock(&mymutex);
   scalarprod += prod;
   pthread_mutex_unlock(&mymutex);
   }


int main(int argc, char **argv)
   {
   CpuTimer TR;       // object to measure execution times
   Rand R(999);         // random generator used to initialize vectors
   
   for(int n=0; n<VECSIZE; n++)
      {
      A[n] = -1.0 + 2.0 * R.draw();      // value in [-1, 1]
      B[n] = -1.0 + 2.0 * R.draw();      // value in [-1, 1]
      }
   
   TR.Start();
   for(int n=0; n<6; n++)      // perform the same job 6 times
      {
      scalarprod = 0.0;
      TH.Dispatch(thread_fct, NULL);
      TH.WaitForIdle();
      }
   TR.Stop();

   cout << "\n Scalar pruduct is = " << scalarprod << endl;
   TR.Report();
   cout << "\n Correct algorithm, no synchronization overhead ";
   cout << "\n Mutex locked only nThreads times per job\n"
        << endl;
   }

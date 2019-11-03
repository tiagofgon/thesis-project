// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// **************************************
// File ScaProd_Tbb.C
// 
// Scalar product of two vectors
//
// In this example, we are using a TBB mutex to accumulate 
// partial results. The "scoped locking" protocol is used
// to leck the mutex and create a critical section. 
//
// We use the SPool pool to run the worker threads.
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
#include <tbb/mutex.h>
#include <iostream>

#define VECSIZE  10000000

using namespace std;

// Global variables
// ----------------
double A[VECSIZE];
double B[VECSIZE];

tbb::mutex my_mutex;
SPool *TH;
double scalarprod;

void task_fct(void *P)
   {
   double d;
   int beg, end;
   beg = 0;                     // initialize [beg, end) to global range
   end = VECSIZE;
   TH->ThreadRange(beg, end);   // now [beg, end) is the range for this
                                // thread
   for(int n=beg; n<end; n++)
      {
      d = A[n]*B[n];
         {
         // ---------< critical section > ---------
         tbb::mutex::scoped_lock my_lock(my_mutex);
         scalarprod += d;
         // ---------------------------------------
         }
      }  
   }

int main(int argc, char **argv)
   {
   CpuTimer TR;         // object to measure execution times
   Rand R(999);         // random generator used to initialize vectors
   int nTh;
 
   if(argc==2) nTh=atoi(argv[1]);
   else nTh = 2;

   TH = new SPool(nTh);
   scalarprod = 0.0;
   for(int n=0; n<VECSIZE; n++)
      {
      A[n] = -1.0 + 2.0 * R.draw();      // value in [-1, 1]
      B[n] = -1.0 + 2.0 * R.draw();      // value in [-1, 1]
      }
   
   TR.Start();
   TH->Dispatch(task_fct, NULL);
   TH->WaitForIdle();
   TR.Stop();

   cout << "\n Scalar product is = " << scalarprod << endl;
   TR.Report();
   cout << "\n NOTICE: enormous synchronization overhead";
   cout << "\n Parallel speedup (2) lost because of important system time"
        << endl;

   delete TH;
   }

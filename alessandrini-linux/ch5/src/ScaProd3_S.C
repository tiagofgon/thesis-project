// **************************************
// Copyright (c) 2015 Victor Alessandrini
// All rights reserved.
// **************************************
// File ScaProd3_S.C
// -------------------------------------------------------
// Scalar product of two vectors. Results are accumulated
// in a local variable.
//
// A C++11 mutex is used, but it is locked once for each 
// thread. This is the correct way of handling this
// problem.
// -------------------------------------------------------
#include <stdlib.h>
#include <CpuTimer.h>
#include <Rand.h>
#include <SPool.h>
#include <mutex>
#include <iostream>
#define VECSIZE  10000000

using namespace std;

// Global variables
// ----------------
double A[VECSIZE];
double B[VECSIZE];
double scalarprod;     

SPool *TH;     // will have two worker threads
std::mutex my_mutex;

void thread_fct(void *P)
   {
   double prod;
   int beg, end;
   beg = 0;                    // initialize [beg, end) to global range
   end = VECSIZE;
   TH->ThreadRange(beg, end);   // now [beg, end) is the range for this
                               // thread
   prod = 0.0;
   for(int n=beg; n<end; n++) prod += A[n]*B[n];
      {
      // critical section to accumulate values of prod
      std::lock_guard<mutex> my_lock(my_mutex);
      scalarprod += prod;
      }
   }


int main(int argc, char **argv)
   {
   CpuTimer TR;       // object to measure execution times
   Rand R(999);         // random generator used to initialize vectors
   TH = new SPool(2);
 
   for(int n=0; n<VECSIZE; n++)
      {
      A[n] = -1.0 + 2.0 * R.draw();      // value in [-1, 1]
      B[n] = -1.0 + 2.0 * R.draw();      // value in [-1, 1]
      }
   
   TR.Start();
   for(int n=0; n<6; n++)      // perform the same job 6 times
      {
      scalarprod = 0.0;
      TH->Dispatch(thread_fct, NULL);
      TH->WaitForIdle();
      }
   TR.Stop();

   cout << "\n Scalar product is = " << scalarprod << endl;
   TR.Report();
   cout << "\n Correct algorithm, no synchronization overhead ";
   cout << "\n Mutex locked only nThreads times per job\n" << endl;
   delete TH;
   }

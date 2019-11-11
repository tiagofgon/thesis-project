// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// **************************************
// File ScaProd1_S.C
// 
// Scalar product of two vectors
//
// An ordinary C++11 mutex is used to protect the global
// variable that accumulates partial results. The mutex is 
// locked every time a partial result is computed (excessive 
// mutex contention).
// --------------------------------------

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
std::mutex mymutex;

SPool TH(2);     // will have two worker threads

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
         {
         std::lock_guard<mutex> my_lock(mymutex);
         scalarprod += d;
         }
      }
   }


int main(int argc, char **argv)
   {
   CpuTimer TR;       // object to measure execution times
   Rand R(999);       // random generator used to initialize vectors
   
   scalarprod = 0.0;
   for(int n=0; n<VECSIZE; n++)
      {
      A[n] = -1.0 + 2.0 * R.draw();      // value in [-1, 1]
      B[n] = -1.0 + 2.0 * R.draw();      // value in [-1, 1]
      }
 
   TR.Start();
   for(int n=0; n<6; n++)   // perform the same job 6 times
      {
      scalarprod = 0.0;
      TH.Dispatch(thread_fct, NULL);
      TH.WaitForIdle();
      }
   TR.Stop();

   cout << "\n Scalar product is = " << scalarprod << endl;
   TR.Report();
   cout << "\n NOTICE: enormous synchronization overhead";
   cout << "\n Parallel speedup (2) lost because of important system time\n"
        << endl;
   }

// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// **************************************
// File McPi.C
// 
// MonteCarlo computation of PI.
// Multithreaded code (2 threads). This code
// uses a global random number generator, and
// it is not thread safe. Repeated executions
// do not reproduce exactly the same result.
// ------------------------------------------
#include <stdlib.h>
#include <CpuTimer.h>
#include <iostream>
#include <Rand.h>
#include <SPool.h>             // new

using namespace std;
SPool TH(2);            // pool of two threads
long accepted[3];       // storage of partial results
long nsamples;          // number of mC events
Rand R(999);            // global random number generator

void thread_fct(void *P)
   {
   unsigned long ct;
   double x, y;
   int rank = TH.GetRank();
   ct = 0;
   for(int n=0; n<nsamples; n++)
      {
      x = R.draw();
      y = R.draw();
      if((x*x+y*y) <= 1.0 ) ct++;
      }
   accepted[rank] = ct;
   }
   

int main(int argc, char **argv)
   {
   CpuTimer T;
   double x, y, pi;

   // Initialize nsamples
   // -------------------
   if(argc==2) nsamples = atoi(argv[1]);
   else nsamples = 1000000000;
   nsamples /= 2;

   // Run, measuring execution times
   // ------------------------------
   T.Start();
   // -------------------------------------------------------
   TH.Dispatch(thread_fct, NULL);
   TH.WaitForIdle();
   pi = 4.0 * (double)(accepted[1]+accepted[2]) / (2*nsamples);
   // -------------------------------------------------------
   T.Stop();

   cout << "\n  accepted[1] = " << accepted[1] << "\n  accepted[2] = "
           << accepted[2] << "\n  Value of PI = " << pi << endl;
   T.Report();
   }

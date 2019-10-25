// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// **************************************
// File CalcPiOmp.C
// 
// MonteCarlo computation of PI
// OpenMP version
//
// Multithreaded code, using a global random number 
// generator (not thread safe).
//
// The effect of thread unsafety is easily seen when 
// there are several threads (choose 8 threads from the 
// command line) and a rather small number of samples
// (change nSamples to 100000).
// ----------------------------------
#include <stdlib.h>
#include <CpuTimer.h>
#include <iostream>
#include <Rand.h>
#include <omp.h>               // new

using namespace std;
long *count;            // storage of partial results
long nsamples;          // number of MC events
Rand R(999);

void task_fct()
   {
   long ct;
   double x, y;
   int rank = omp_get_thread_num();
   std::cout << "\n Thread of rank " << rank << " active" << std::endl;

   ct = 0;
   for(int n=0; n<nsamples; n++)
      {
      x = R.draw();
      y = R.draw();
      if((x*x+y*y) <= 1.0 ) ct++;
      }
   count[rank] = ct;
   }
   

int main(int argc, char **argv)
   {
   CpuTimer TR;
   double x, y, pi;
   int nTh;
   long cnt;

   nTh = 2;                          // default values
   nsamples = 1000000000;
   // --------------------
   if(argc==2) nTh = atoi(argv[1]);  // change nTh defaults
   if(argc==3)                       // change nTh and nsamples
      {
      nTh = atoi(argv[1]);
      nsamples = atoi(argv[2]);
      }
   count = new long[nTh];
   nsamples /= nTh;

   TR.Start();
   omp_set_num_threads(nTh);
   #pragma omp parallel
      {
      task_fct();
      }

   cnt = 0;
   for(int n=0; n<nTh; ++n)   // cumulate partial results
      {
      cnt += count[n];
      cout << "\n count[" << n <<"] = " << count[n];
      }
   cout << endl;

   pi = 4.0 * (double)cnt / (nTh*nsamples);
   // ---------------------------------------------------
   TR.Stop();
   cout << "\n Value of PI = " << pi << endl;
   TR.Report();
   }

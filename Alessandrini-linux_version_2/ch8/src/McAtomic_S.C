// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File McAtomic_S.C
// 
// MonteCarlo computation of PI. This is a simple
// example, in which a global std::atomic counter is used 
// to accumulate acceptances. There is excessive mutual
// exclusion: the counter is increased every time there
// is a naw accepted event. But the performances are 
// acceptable.
//
// Multithreaded code (2 threads)
// ----------------------------------
#include <stdlib.h>
#include <CpuTimer.h>
#include <iostream>
#include <Rand.h>
#include <SPool.h>             
#include <atomic>

using namespace std;

SPool TH(2);                   // set of two threads
long nsamples;                 // number of MonteCarlo events per thread
std::atomic<long> C;           // atomic<long> to accumulate acceptances


void thread_fct(void *P)
   {
   double x, y;
   int rank;

   rank = TH.GetRank();
   Rand R(rank*999);
   cout << "\n Thread started, rank = " << rank << endl;
   for(int n=0; n<nsamples; n++)
      {
      x = R.draw();
      y = R.draw();
      if((x*x+y*y) <= 1.0 ) ++C;    // accumulate in atomic<int>
      }
   }
   
int main(int argc, char **argv)
   {
   CpuTimer TR;
   double pi, d;

   if(argc==2) nsamples = atoi(argv[1]);
   else nsamples = 10000000;
   
   TR.Start();
   TH.Dispatch(thread_fct, NULL);
   TH.WaitForIdle();
   d = (double) C;
   pi = 4.0 * d / (2*nsamples);
   TR.Stop();
   TR.Report();

   cout << "\n Value of PI = " << pi << endl;
   }

// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File CalcPiOmp.C
// 
// MonteCarlo computation of PI.
//
// Yet another version of the parallel code, to show 
// the flexibility of the OpenMP environment. 
// -------------------------------------------------
#include <stdlib.h>
#include <omp.h>
#include <Rand.h>
#include <iostream>

using namespace std;

// -------------------------------------------------
// This is an OpenMP routine that is called by each
// worker thread in the parallel region to compute
// the partial acceptance of the MC run
// -------------------------------------------------
unsigned long GetAcceptance(long NS)
   {
   double x, y;
   unsigned long mycount;

   int rank = omp_get_thread_num()+1;
   Rand R(999*rank);
   for(size_t n=0; n<NS/2; n++)
      {
      x = R.draw();
      y = R.draw();
      if((x*x+y*y) <= 1.0 ) mycount++;
      }
   return mycount;
   }

int main(int argc, char **argv)
   {
   long nsamples;
   double pi;
   unsigned long count;

   // get nsamples from command line
   // ------------------------------
   if(argc==2) nsamples = atoi(argv[1]);
   else nsamples = 10000000;

   count = 0; 
   omp_set_num_threads(2);

   // ---------------------------------------------------------------
   // In this parallel region, each thread calls an auxiliary function
   // to compute its acceptance, and then a reduction is performed on
   // the partial acceptances
   // ---------------------------------------------------------------
   #pragma omp parallel firstprivate(nsamples) reduction(+:count) 
      { count = GetAcceptance(nsamples); }

   // Here "counts" holds the total acceptance
   // ----------------------------------------
   pi = 4.0 * (double)(count) / nsamples;
   cout << "\n Value of PI = " << pi << endl;
   }

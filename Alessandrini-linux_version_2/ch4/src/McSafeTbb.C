// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ***********************************************
// File McSafeTbb.C
// 
// MonteCarlo computation of PI
//
// Thread safe parallel version of  McPi.C, using SPool 
// and the TBB thread local strorage facility
// ----------------------------------------------------

#include <iostream>
#include <stdlib.h>
#include <SPool.h>
#include <CpuTimer.h>
#include <tbb/enumerable_thread_specific.h>

using namespace std;

// Global variables
// ----------------
int   nTh;
SPool *TH;
long  *accepted;
long  nsamples;

/* ------------------------------------------------------
 * This is an auxiliary function used by TBB to initialize 
 * an enumerable_thread_specific<int> object
 * -----------------------------------------------------*/
int finit()
   {
   int retval;
   retval = 999 * TH->GetRank();
   return retval;
   }

// ----------------------------------------------
// Auxiliary function :  a simple generator, that 
// produces uniform deviates in [0,1]. The integer 
// "seed" is preserved between calls. 
// ----------------------------------------------
//
#define PI     3.1415926535
#define IMUL   314159269
#define IADD   453806245
#define MASK   2147483647
#define SCALE  0.4656612873e-9

tbb::enumerable_thread_specific<int> seed(finit);    

double Rand()
   {
   // In the statement below, "my_seed" is a reference to the
   // value of "seed" owned by the caller thread:
   // -------------------------------------------------------
   tbb::enumerable_thread_specific<int>::reference my_seed = seed.local();

   int retval = (my_seed * IMUL + IADD) & MASK;
   my_seed = retval;
   return (retval * SCALE);
   }

// The task function
// -----------------
void task_fct(void *P)
   {
   double x, y;
   long count; 
   int rank = TH->GetRank();

   count = 0;
   for(size_t n=0; n<nsamples; n++)
      {
      x = Rand();
      y = Rand();
      if((x*x+y*y) <= 1.0 ) count++;
      }
   accepted[rank] = count;
   }

// The main function
// -----------------
int main(int argc, char **argv)
   {
   long C;
   CpuTimer T;

   if(argc==2) nTh = atoi(argv[1]);
   else nTh = 2;

   TH = new SPool(nTh);
   accepted = new long[nTh+1];
   nsamples = 1000000000;

   T.Start();
   TH->Dispatch(task_fct, NULL);
   TH->WaitForIdle();
   T.Stop();

   C = 0;
   for(int n=1; n<=nTh; ++n) C += accepted[n];
 
   double pi = 2.0 * (double)C / nsamples;
   cout << "\n Value of PI = " << pi << endl;
   T.Report();
   }

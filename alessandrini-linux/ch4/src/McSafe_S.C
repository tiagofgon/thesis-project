// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ***********************************************
// File McStd_S.C
// 
// MonteCarlo computation of PI
//
// Thread safe parallel version of the MC computation 
// of PI, using the C++11 "thread_local" keyword. This
// is a very powerful utility, very close to the OpenMP 
// version. 
// ---------------------------------------

#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <SPool.h>
#include <CpuTimer.h>

using namespace std;

#define PI     3.1415926535
#define IMUL   314159269
#define IADD   453806245
#define MASK   2147483647
#define SCALE  0.4656612873e-9

// Global variables in this example
// --------------------------------
int   nTh;
SPool *TH;
long  *accepted;
long  nsamples;

// -----------------------------------------------------------------
// Global generator, rendered thread safe by the "thread_local"
// C++11 keyword. The initialization statement of "seed" is executed
// the first time a thread calls this function
//
// Not all compilers implement the thread_local keyword. Visual
// Studio 2013 does not accept this code. GNU 4.8 is fine.
// -----------------------------------------------------------------
double Rand()
   {
   // notice the simplicity of this initialization
   thread_local int my_seed = 999 * TH->GetRank();
      
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
   double pi = 4.0 * (double)C / (nsamples*nTh);
   cout << "\n Value of PI = " << pi << endl;
   T.Report();
   }

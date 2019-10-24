// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ***********************************************
// File McSafeOmp.C
// 
// MonteCarlo computation of PI
//
// Thread safe parallel version. A global random 
// number generator is rendered thread safe with
// the OpenMP threadprivate directive
// ----------------------------------------------

#include <iostream>
#include <stdlib.h>
#include <CpuTimer.h>
#include <omp.h>

using namespace std;

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

// ---------------------------------------------------
// Thread safe generator. The initialization statement
// of seed is executed the first time a thread calls 
// this function. This dynamic initialization is not
// implemented by all compilers: GNU 4.8 is OK, but
// Visual Studio 2013 is not. Look at the Windows version
// of this example to see the workaround we adopted
// ------------------------------------------------------
double Rand()
   {
   static int seed = 999 * (omp_get_thread_num()+1);
   #pragma omp threadprivate(seed)      // notice the simplicity
       
   seed = (seed * IMUL + IADD) & MASK;
   return (seed * SCALE);
   }

// Global variables
// ----------------
int   nTh;
long  nsamples;
long  accepted[2];   // OK, ranks start from 0 here

// The task function
// -----------------
void task_fct()
   {
   double x, y;
   long count; 
   int rank;

   rank = omp_get_thread_num();
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

   nsamples = 1000000000;
   omp_set_num_threads(2);

   T.Start();
   #pragma omp parallel
      { 
      task_fct(); 
      }
   T.Stop();

   C = 0;
   for(int n=0; n<2; ++n) C += accepted[n];
   double pi = 2.0 * (double)C / (nsamples);
   cout << "\n Value of PI = " << pi << endl;
   T.Report();
   }

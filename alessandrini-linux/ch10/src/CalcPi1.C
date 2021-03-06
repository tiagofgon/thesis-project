// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File CalcPi1.C
// -------------------------------------------------
// MonteCarlo computation of PI.
//
// The purpose of this example is to develop a parallel 
// version of the CalcPi.C code, just by  inserting 
// directives in the sequential code.
//
// Notice in particular the usage of the "reduction" 
// clause in the "parallel" directive.
// -------------------------------------------------
#include <stdlib.h>
#include <omp.h>
#include <iostream>

using namespace std;

// ----------------------------------------------
// Auxiliary function :  a simple generator, that 
// produces uniform deviates in [0,1]. The integer 
// "seed" is preserved between calls. 
// ----------------------------------------------
#define PI     3.1415926535
#define IMUL   314159269
#define IADD   453806245
#define MASK   2147483647
#define SCALE  0.4656612873e-9

double Rand()
   {
   int rank = omp_get_thread_num();
   static int seed = 999*(rank+1);          // internal state variable
   #pragma omp threadprivate(seed)      // make this function thread safe

   seed = (seed * IMUL + IADD) & MASK;
   return (seed * SCALE);
   }

int main(int argc, char **argv)
   {
   long nsamples;
   double pi;
   unsigned long count;

   if(argc==2) nsamples = atoi(argv[1]);
   else nsamples = 10000000;

   count = 0; 
   omp_set_num_threads(2);
   #pragma omp parallel firstprivate(nsamples) reduction(+:count) 
      {
      // task function ------------------------
      double x, y;
      for(size_t n=0; n<nsamples; n++)
         {
         x = Rand();
         y = Rand();
         if((x*x+y*y) <= 1.0 ) count++;
         }
      // --------------------------------------
      }
   pi = 4.0 * (double)(count) / (2 * nsamples);

   cout << "\n Value of PI = " << pi << endl;
   }

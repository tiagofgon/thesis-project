// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File CalcPi.C
//
// MonteCarlo computation of PI
//
// Sequential code, but prepared to be modified by
// adding OpenMP parallelization directives.
// -----------------------------------------------

#include <iostream>
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
double Rand()
   {
   static int seed = 999;               // internal state variable
   seed = (seed * IMUL + IADD) & MASK;
   return (seed * SCALE);
   }


int main(int argc, char **argv)
   {
   long nsamples;
   double x, y, pi;
   unsigned long count;

   nsamples = 10000000;
   count = 0; 

   for(size_t n=0; n<nsamples; n++)
      {
      x = Rand();
      y = Rand();
      if((x*x+y*y) <= 1.0 ) count++;
      }
   pi = 4.0 * (double)(count) / nsamples;
   cout << "\n Value of PI = " << pi << endl;
   }

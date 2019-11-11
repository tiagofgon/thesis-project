// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ***********************************************
/* File Gromp.h
 *
 * Gaussian random generator using the Box-Muller
 * algorithm. Thread safety is implemented by using
 * the OpenMP threadprivate directive
 * ----------------------------------------------*/
#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <omp.h>

/* -------------------------------------------
 * Auxiliary function :  a simple generator, that 
 * produces uniform deviates in [0,1]. The integer 
 * "seed" is preserved between calls. 
 * ------------------------------------------*/
#define PI     3.1415926535
#define IMUL   314159269
#define IADD   453806245
#define MASK   2147483647
#define SCALE  0.4656612873e-9

double Rand()
   {
   static int seed = 999 * (omp_get_thread_num()+1);
   #pragma omp threadprivate(seed)  

   seed = (seed * IMUL + IADD) & MASK;
   return (seed * SCALE);
   }


/* --------------------------------------
 * This is the Gaussian generator. Two
 * static variables "ransave" and "flag",
 * are preserved between calls.
 * -------------------------------------*/

double Grand()
   {
   static double ransave = 0.0;
   static int flag = 0;
   #pragma omp threadprivate(ransave, flag)
   
   double x1, x2, scratch;
   if(flag)
      {
      flag = 0;
      return ransave;
      }
   else
      {
      x1 = Rand();
      x2 = Rand();
      scratch = sqrt(-2*log(x1));
      x1 = cos(2 * PI * x2);
      x2 = sin(2 * PI * x2);
      ransave = scratch * x2;
      flag = 1;
      return(scratch * x1);
      }
   }

// -------------------------------------
// Global variables for the application 
// -------------------------------------  
double presult[2];       // storage of thread partial results
long   nSamples;

// ---------------------
// The thread function
// ---------------------
void th_function()
   {
   double rand, reduct;
   int rank = omp_get_thread_num();
   long nS2 = nSamples/2;

   reduct = 0.0;
   for(long n=0; n<nS2; ++n)
      {
      rand = Grand();
      reduct += (rand*rand);
      }
   reduct /= nS2;
   presult[rank] = reduct;   // store partial result
   } 


/* ------------------
 * The main function
 * ------------------*/
int main(int argc, char **argv)
   {

   if(argc==2) nSamples = atol(argv[1]);
   else nSamples = 10000000;

   omp_set_num_threads(2);
   #pragma omp parallel
      {  th_function(); }

   double result = 0.5 * (presult[0]+presult[1]);
   std::cout << "\n Variance = " << result << std::endl;
   }


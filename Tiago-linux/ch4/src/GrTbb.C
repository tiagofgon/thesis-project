// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ***********************************************
/* File GrTbb.h
 *
 * Gaussian random generator using the Box-Muller
 * algorithm.
 * Using the TBB enumerable_thread_specific<T> class 
 * for thread local storage
 * -------   ---------------------------------------*/

#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <SPool.h>
#include <tbb/enumerable_thread_specific.h>

#define PI     3.1415926535
#define IMUL   314159269
#define IADD   453806245
#define MASK   2147483647
#define SCALE  0.4656612873e-9

int finit();    // function declaration

// -------------------------------------
// Global variables for the application 
// -------------------------------------  
SPool TH(2);          // 2 worker threads
double presult[3];    // storage of thread partial results
long   nSamples;

// --------------------------------------------------------------
// Here we have the thread local containers. The first one is for 
// the seed of the standard generator, and it requires an explicit 
// thread specific initialization via the function finit.
// The two others provide thread local storage for the Box-Muller
// generator, and the default initialization to 0 is sufficient.
// ---------------------------------------------------------------
tbb::enumerable_thread_specific<int>    seed(finit);  
tbb::enumerable_thread_specific<double> ransave;
tbb::enumerable_thread_specific<int>    flag;     

// --------------------------------------------------
// Auxiliary function used by TBB to initialize local 
// instances of a thread local variables. 
// --------------------------------------------------
int finit()
   {
   int retval;
   retval = 999 * TH.GetRank();
   return retval;
   }

// -------------------------------------------
// Generator of uniform deviates in [0,1]. 
// -------------------------------------------
double Rand()
   {
   tbb::enumerable_thread_specific<int>::reference my_seed = seed.local();
   int retval = (my_seed * IMUL + IADD) & MASK;
   my_seed = retval;
   return (my_seed * SCALE);
   }

// -------------------------
// Gaussian random generator
// -------------------------
double Grand()
   {
   tbb::enumerable_thread_specific<int>::reference my_flag = flag.local();
   tbb::enumerable_thread_specific<double>::reference my_ransave = 
                                                      ransave.local();
   double x1, x2, scratch;
   
   if(my_flag)
      {
      my_flag = 0;
      scratch = my_ransave;
      return scratch;
      }
   else
      {
      x1 = Rand();
      x2 = Rand();
      scratch = sqrt(-2*log(x1));
      x1 = cos(2 * PI * x2);
      x2 = sin(2 * PI * x2);
      my_ransave = scratch * x2;
      my_flag = 1;
      return(scratch * x1);
      }
   }

// ---------------------
// The thread function
// ---------------------
void th_function(void *P)
   {
   double rand, reduct;
   int rank = TH.GetRank();
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
   TH.Dispatch(th_function, NULL);
   TH.WaitForIdle();
   double result = 0.5 * (presult[1]+presult[2]);
   std::cout << "\n Variance = " << result << std::endl;
   }


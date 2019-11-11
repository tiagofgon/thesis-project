// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ***********************************************
/* File GrSafe.h
 * Gaussian random generator using the Box-Muller
 * algorithm.
 *
 * The hard way: state variables are factored out of 
 * the generator code, and they are passed through 
 * the function argument. This makes the generator 
 * thread safe.
 * ----------------------------------------------*/
#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <SPool.h>

struct Gstate   // encapsulates a persistent state
   {
   int seed;
   int flag;
   double ransave;
   };

/* -------------------------------------------
 * Auxiliary function :  a simple generator, that 
 * produces uniform deviates in [0,1]. 
 * ------------------------------------------*/
#define PI     3.1415926535
#define IMUL   314159269
#define IADD   453806245
#define MASK   2147483647
#define SCALE  0.4656612873e-9

double Rand (int *seed)
   {
   *seed = (*seed * IMUL + IADD) & MASK;
   return (*seed * SCALE);
   }

/* -----------------------------------------------------
 * This is the Gaussian generator. It receives the state
 * as function argument. No static variables are maintained
 * between calls.
 * ----------------------------------------------------*/
double Grand(Gstate *GS)
   {
   double x1, x2, scratch;
   
   if(GS->flag)
      {
      GS->flag = 0;
      return GS->ransave;
      }
   else
      {
      x1 = Rand(&(GS->seed));
      x2 = Rand(&(GS->seed));
      scratch = sqrt(-2*log(x1));
      x1 = cos(2 * PI * x2);
      x2 = sin(2 * PI * x2);
      GS->ransave = scratch * x2;
      GS->flag = 1;
      return(scratch * x1);
      }
   }

// -------------------------------------
// Global variables for the application 
// -------------------------------------  
SPool TH(2);             // 2 worker threads
double presult[3];       // storage of thread partial results
long   nSamples;
   
// ---------------------
// The thread function
// ---------------------
void th_function(void *P)
   {
   double rand, reduct;
   int rank = TH.GetRank();
   long nS2 = nSamples/2;
   Gstate GS;             // Proprietary internal state of generator

   // Initialize state  variables
   // ---------------------------
   GS.seed = (rank+1)*999;   // thread dependent initial seed
   GS.flag = 0;              // no previous result

   reduct = 0.0;
   for(long n=0; n<nS2; ++n)
      {
      rand = Grand(&GS);
      reduct += (rand*rand);
      }
   reduct /= nS2;
   presult[rank] = reduct;   // store partial result
   } 


/* ------------------
 * The main function.
 * Same as before
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
 

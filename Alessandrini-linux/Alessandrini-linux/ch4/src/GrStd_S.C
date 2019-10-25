// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ***********************************************
/* File GrStd_S.C
 *
 * Gaussian random generator using the Box-Muller
 * algorithm.
 * Thread safety is implemented with the C++11 
 * thread_local keyword.
 * -------   ---------------------------------------*/
#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <SPool.h>

#define PI     3.1415926535
#define IMUL   314159269
#define IADD   453806245
#define MASK   2147483647
#define SCALE  0.4656612873e-9

// -------------------------------------
// Global variables for the application 
// -------------------------------------  
SPool *TH;            
double presult[3];    // storage of thread partial results
long   nSamples;

// -------------------------------------------
// Generator of uniform deviates in [0,1]. 
// -------------------------------------------
double Rand()
   {
   thread_local int my_seed = 999 * TH->GetRank();      
   int retval = (my_seed * IMUL + IADD) & MASK;
   my_seed = retval;
   return (retval * SCALE);
   }


// -------------------------
// Gaussian random generator
// -------------------------
double Grand()
   {
   double x1, x2, scratch;
   thread_local int my_flag = 0;   // initialization to 0 is OK
   thread_local double my_ransave = 0.0;
   
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
void Th_Fct(void *P)
   {
   double rand, reduct;
   int rank = TH->GetRank();
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
   TH = new SPool(2);

   // -----------------------------
   TH->Dispatch(Th_Fct, NULL);
   TH->WaitForIdle();
   // -----------------------------

   double result = 0.5 * (presult[1]+presult[2]);
   std::cout << "\n Variance = " << result << std::endl;
   delete TH;
   }


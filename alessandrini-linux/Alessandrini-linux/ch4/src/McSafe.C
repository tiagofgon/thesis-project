// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ***********************************************
// File McSafe.C
// 
// MonteCarlo computation of PI
//
// Thread safe version, the hard way: state variables 
// are factored out of the generator code, and they are 
// passed through the function argument. This makes the 
// generator thread safe.

// -----------------------------------------------

#include <iostream>
#include <stdlib.h>
#include <SPool.h>
#include <CpuTimer.h>

using namespace std;

// --------------------------------------------------
// A simple generator, that produces uniform deviates 
// in [0,1]. The integer "seed" is preserved between 
// calls. 
// --------------------------------------------------

int  IMUL = 314159269;
int  IADD = 453806245;
int  MASK = 2147483647;
double SCALE = 0.4656612873e-9;

double Rand(long& seed)
    {
    seed = (seed * IMUL + IADD) & MASK;
    return (seed * SCALE);
    }


// Global variables for this example
// ---------------------------------
int  nTh;
SPool TH(2);           // pool of two threads
long nsamples;         // number of MC events
long accepted[3];      // storage of partial results

// The task function
// -----------------
void task_fct(void *P)
   {
   double x, y;
   long count;
   long my_seed;          // each thread owns its seed ...
   int rank;

   rank = TH.GetRank();   // in SPool, ranks start at 1
   my_seed = 999*rank;    // thread dependent initialization
   count = 0;
   for(size_t n=0; n<nsamples; n++)
      {
      x = Rand(my_seed);
      y = Rand(my_seed);
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
   nsamples = 1000000000;

   T.Start();
   TH.Dispatch(task_fct, NULL);
   TH.WaitForIdle();
   T.Stop();

   C = accepted[1]+accepted[2];
   double pi = 2.0 * (double)C / nsamples;
   cout << "\n Value of PI = " << pi << endl;
   T.Report();
   }

// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// **************************************
// File GrObj.C
//
// Implementation of gaussuan random generator, using
// Box-Muller algorithm. It requires a uniform generator
// in [0,1], which is the function RandomR given below.
//
// The gaussian random generator is implemented as a
// C++ class.
// *****************************************************

#include <iostream>
#include <stdlib.h>
#include <GaussGen.h>
#include <SPool.h>

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
   GaussGen G(999*rank);      // local generator

   reduct = 0.0;
   for(long n=0; n<nS2; ++n)
      {
      rand = G.Draw();
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
 
   
//////////////////////////////////////////////////////////

// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// This is file HeatTBB.C
//
// Microtasking TBB implementation, using
// parallel_reduce
// -------------------------------------- 
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <tbb/tick_count.h>
#include "tbb/parallel_for.h"
#include "tbb/parallel_reduce.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/blocked_range.h"
#include "tbb/tbb_stddef.h"

using namespace tbb;

// ----------------------------------
// Declaration of auxiliary functions
// ----------------------------------
void InputData();
void InitJob(int MM, int NN);
void ExitJob();
void PrintResult(int n);

// -----------------
// Global variables
// -----------------

double **U, **V;
const double EPS = 1.0e-5;
double initial_error, curr_error;
int nIter;
int    G;
int    moreCycles;

// ------------------------------
// Data read from file "heat.dat"
// ------------------------------
int N, M;
int maxIts;
int nThreads;                // new
int stepReport;


// ---------------------------------------------
// Auxiliary function to prepare next iteration,
// and decide if it must take place
// ---------------------------------------------
bool NextIteration(double error)
   {
   double **swap = U;   // swap pointers
   U = V;
   V = swap;
   nIter++;
   if(nIter%stepReport==0) std::cout << "\n Iteration " << nIter << std::endl;
   if( (error > EPS*initial_error) && (nIter <= maxIts) ) return true;
   else return false;
   }


// --------------------------------------------- 
// The "Body" class used for the parallel_reduce
// TBB alforithm.
// ---------------------------------------------
class DomainSwap
   {
   public:
   double error_norm;

   // Constructor
   DomainSwap() : error_norm(0.0) {}

   // Split constructor
   DomainSwap(DomainSwap& dom, split toto) : error_norm(0.0) {}

   // Update V using the U data
   void operator() (const blocked_range<size_t>& rg)
      {
      for(size_t m=rg.begin(); m!=rg.end(); m++)
            {
            for(size_t n=1; n<(N-1); n++)
               {
               double resid = U[m+1][n] + U[m-1][n] + U[m][n-1] + U[m][n+1]
		               - 4 * U[m][n];
	       error_norm += fabs(resid);
               V[m][n] = U[m][n] + 0.25 * resid;    
	       }
	    }
      }		 

   void join(const DomainSwap& dom) 
      {
      error_norm += dom.error_norm;
      }
   };


int main(int argc, char **argv)
   {
   double **swap;

   InputData();
   InitJob(M, N);

   task_scheduler_init init(nThreads);
   G = M/nThreads;
   if(argc==2) G = atoi(argv[1]);

   tbb::tick_count t0 = tbb::tick_count::now(); 
   do
      {
      DomainSwap DS;
      parallel_reduce(blocked_range<size_t>(1, M-1, G), DS);
      curr_error = DS.error_norm;
      moreCycles = NextIteration(curr_error);
      }while(moreCycles); 

   // -------------
   // Output result
   // -------------
   tbb::tick_count t1 = tbb::tick_count::now(); 
   printf("\n\n Data: N = %d  M = %d   maxIts = %d\n", N, M, maxIts);
   if(nIter >maxIts) printf("\n Maximal number of iterations exceeded\n");
   else 
      {
      printf("\n Initial error  : %g", initial_error);
      printf("\n Final error    : %g", curr_error);
      printf("\n Iterations     : %d", nIter);
      printf("\n Active threads : %d\n", nThreads);
      std::cout << "\n Wall time = " << (t1-t0).seconds() << " seconds"
                << std::endl;
      }
   ExitJob();
   return 0;
   }

/********************************************************/

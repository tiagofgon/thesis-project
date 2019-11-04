// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
/* This is file SorTBB.C
 * 
 * Implements Successive Overrelaxation with Tchebichef 
 * accelleration for solving a two dimensional boundary
 * value problem.
 *
 * Microtasking version, using TBB parallel_reduce 
 * algorithm to perform a lattice swap and the error
 * reduction.
 * ====================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>
#include <tbb/tick_count.h>
#include "tbb/parallel_reduce.h"
#include "tbb/task_scheduler_init.h"
#include "tbb/blocked_range.h"
#include "tbb/tbb_stddef.h"

using namespace tbb;

// ------------------------------------
// Declaration of auxiliary functions
// used by the code
// ------------------------------------

void   InputData();
void   InitJob(int MM, int NN);
void   ExitJob();
void   PrintData();

// ---------------------
// Internal global data 
// ---------------------

double **U, **F;
double anorm, anormf, delta, omega;
const double EPS = 1.0e-5;
const double PI  = 3.1415926535;
int    nIter;             // iteration counter
int    G;                 // granularity

// -----------------------------
// Data read from file "sor.dat"
// -----------------------------

int N, M, Lgth;
int maxIts, nTh, stepReport;

// --------------------------------------------- 
// The "Body" class used for the parallel_reduce
// TBB alforithm. The constructor of the class has
// a "pass" argument that distinghuishes the first
// and the second swap when updating the solution.
// ---------------------------------------------
class DomainSwap
   {
   private:
   int pass;

   public:
   double error_norm;

   // Ordinary and split constructors
   // -------------------------------
   DomainSwap(int p) : pass(p), error_norm(0.0) {}
   DomainSwap(DomainSwap& dom, split toto) : pass(dom.pass), error_norm(0.0) {}

   void operator() (const blocked_range<size_t>& rg)
      {
      for(size_t m=rg.begin(); m!=rg.end(); m++)
         {
         int offset = (m + pass/2) % 2;
         for(int n= offset+1; n<N; n+=2)
            {
            double resid = U[m+1][n] + U[m-1][n] + U[m][n-1] + U[m][n+1]
                           - 4 * U[m][n] - F[m][n];
            error_norm += fabs(resid);
            U[m][n] += 0.25 * omega * resid;   
            }
         }
      }		 

   void join(const DomainSwap& dom) 
      {
      error_norm += dom.error_norm;
      }
   };

// ******************************************************

int main(int argc, char **argv)
   {
   int n, status;

   InputData();
   InitJob(M, N);

   // Initialization of TBB scheduler and granularity
   if(argc==2) nTh = atoi(argv[1]);
   task_scheduler_init init(nTh);
   G = M/nTh;
   omega = 1.8;

   tbb::tick_count t0 = tbb::tick_count::now();
   // -----------------------------------------------------
   do
      {
      // pass 1
      // ------
      DomainSwap DS1(1);
      parallel_reduce(blocked_range<size_t>(1, M-1, G), DS1);
      anorm = DS1.error_norm;
      // pass 2
      // ------
      DomainSwap DS2(2);
      parallel_reduce(blocked_range<size_t>(1, M-1, G), DS2);
      anorm += DS2.error_norm;
      
      nIter++;
      if(nIter%stepReport==0) printf("\n Iteration %d done", nIter);
      }while( (anorm > EPS*anormf) && (nIter <= maxIts) );
   // ---------------------------------------------------------
   tbb::tick_count t1 = tbb::tick_count::now();

   PrintData();
   std::cout << "\n Wall time = " << (t1-t0).seconds() << " seconds"
             << std::endl;
   ExitJob();
   return 0;
   }

//---------------------
// Auxiliary functions
//---------------------
void PrintData()
   {
   printf("\n\n Data: N = %d  M = %d   maxIts = %d", N, M, maxIts);
   if(nIter >maxIts) printf("\n Maximal number of iterations exceeded\n");
   else 
      {
      printf("\n Initial error  : %g", anormf);
      printf("\n Final error    : %g", anorm);
      printf("\n Iterations     : %d", nIter);
      printf("\n Active threads : %d\n", nTh);
      }
   }


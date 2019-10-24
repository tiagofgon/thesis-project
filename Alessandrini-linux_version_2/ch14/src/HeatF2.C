// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File HeatF2.C
// ------------
//
// OpenMP microtasking aapproach. Code remains in sequential
// mode, and loops are parallelized as they show up.

// Same as HeatF.C, but adding two options:
// 
// 1) Add the collapse(2) clause to the external loop parallel
//    for directive. To do this, define OMP_COLLAPSE
// 2) Otherwise, the two loops by hand, and the parallel for 
//    directive acts on the single collapsed loop
//
// You will find out that this is not very satisfactory. The problem
// comes from the boundary conditions, that exclude grid points from
// the update and disrupt the smooth collapse of the the two loops.
// ----------------------------------------------------------
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <Rand.h>
#include <omp.h>
#include <CpuTimer.h>

using namespace std;

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

// ------------------------------
// Data read from file "heat.dat"
// ------------------------------
int N, M;
int maxIts;
int nThreads;
int stepReport;

int main(int argc, char **argv)
   {
   int m, n, status;
   double error_norm, resid;
   double **swap;
   double *Uvec, *Vvec;
   bool test1;
   bool test2;

   InputData();
   InitJob(M, N);
   CpuTimer TR;
   
   TR.Start(); 
   do
      {
      error_norm = 0.0;
      Uvec = U[0];
      Vvec = V[0];
     
      #ifdef OMP_COLLAPSE 
      #pragma omp parallel for num_threads(nThreads) collapse(2) reduction(+:error_norm)
      for(m=1; m<M-1; m++)
            {
            for(n=1; n<(N-1); n++)
               {
               resid = Uvec[(m+1)*N+n] + Uvec[(m-1)*N+n] + Uvec[m*N+n-1] 
                       + Uvec[m*N+n+1] - 4 * Uvec[m*N+n];
	       error_norm += fabs(resid);
               Vvec[m*N+n] = Uvec[m*N+n] + 0.25 * resid;    
	       }
	    }
      #else
      // ----------------------------------------------------
      // Here, the loops are collapsed by hand. The external
      // loop that follows exludes the upper and lower
      // boundaries. Lateral boundaries are excluded by the
      // condition inside the loop: excluding values of m
      // and (m-1) multiples of N  
      // ----------------------------------------------------
      #pragma omp parallel for num_threads(nThreads) reduction(+:error_norm)
      for(m=N; m<(M-1)*N; m++)
            {
            test1 = (m%N != 0);
            test2 = ((m-1)%N != 0);
            if(test1 && test2)
                {
                resid = Uvec[m+N] + Uvec[m-N] + Uvec[m-1] + Uvec[m+1] 
                        - 4 * Uvec[m];
	        error_norm += fabs(resid);
                Vvec[m] = Uvec[m] + 0.25 * resid;    
	        }
             }
      #endif

      // ----------------------------------
      // Swap pointers, so that U is always
      // the current solution
      // ----------------------------------
      swap = U;
      U = V;
      V = swap;
      		 
      curr_error = error_norm;
      nIter++;
      if(nIter%stepReport==0) cout << "\n Iteration " << nIter << " done" 
                                   << endl;
      }while( (curr_error > EPS*initial_error) && (nIter <= maxIts) );
   TR.Stop();

   // -------------
   // Output result
   // -------------
   cout << "\n\n Data: N = " << N << "   M = " << M 
        << "  maxIts = " << maxIts  << endl;
   if(nIter >maxIts) cout << "\n Maximal iterations exceeded" << endl;
   else 
      {
      cout << "\n Initial error " << initial_error;
      cout << "\n Final error   " << curr_error;
      cout << "\n Iterations    " << nIter;
      TR.Report();
      }

   PrintResult(10);
   ExitJob();
   }

    

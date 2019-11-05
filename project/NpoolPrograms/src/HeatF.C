// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File HeatF.C
// ------------
// OpenMP microtasking approach. Code remains in sequential
// mode, and loops are parallelized as they show up.
// --------------------------------------------------------
//
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
int   nIter;
bool  moreCycles;

// ------------------------------
// Data read from file "heat.dat"
// ------------------------------
int N, M;
int maxIts;
int nThreads;
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
   if(nIter%stepReport==0) cout << "\n Iteration " << nIter << endl;
   if( (error > EPS*initial_error) && (nIter <= maxIts) ) return true;
   else return false;
   }

int main(int argc, char **argv)
   {
   int m, n, status;
   double error_norm, resid;
   double **swap;

   InputData();
   InitJob(M, N);
   CpuTimer TR;
  
   if(argc==2) nThreads = atoi(argv[1]); 
   omp_set_num_threads(nThreads);
   TR.Start(); 
   do
      {
      error_norm = 0.0;
      // --------------------------------------------------------------
      // Here, the external loop is parallelized. The internal loop is
      // not. There are two options for the inner loop:
      // 1) collapse the two nested loops into one huge loop with the
      //    "collapse" clause in the parallel for firective
      // 2) vectorize the inner loop (in this case, the best option)
      //
      // Other versions of this code explore these options
      // --------------------------------------------------------------
      #pragma omp parallel for schedule(static) reduction(+:error_norm)
      for(m=1; m<M-1; m++)
         {
         for(n=1; n<(N-1); n++)
            {
               resid = U[m+1][n] + U[m-1][n] + U[m][n-1] + U[m][n+1]
                     - 4 * U[m][n];
               error_norm += fabs(resid);
               V[m][n] = U[m][n] + 0.25 * resid;    
            }
	      }

      curr_error = error_norm; 
      moreCycles = NextIteration(curr_error);
      }while(moreCycles);
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

    

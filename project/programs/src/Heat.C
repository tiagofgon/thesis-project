// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File Heat.C
// 
// Very simple problem : computation of temperatute
// distribution in 2-dimensional square domain with
// simple boundary conditions.
//
// Iterative Jacobi method, not the most efficient one one,
// (Gauss-Seidel with overrelaxation is much better). But this 
// is the simplest method for a parallel implementation, because
// there are no data dependencies.
//
// 2D array of M rows and N columns. Here are the boundary
// conditions:
//
//       n=0                                 n=N-1
//  m=0   * * * * * * * * * * * * * * * * * * * 
//  m=2   * * * * * * * * * * * * * * * * * * * 
//        * * * * * * * * * * * * * * * * * * * 
//        * * * * * * * * * * * * * * * * * * *
//        * * * * * * * * * * * * * * * * * * *
//        * * * * * * * * * * * * * * * * * * *
//  m=M-1 * * * * * * * * * * * * * * * * * * *
//
//  T = 1 on the front border (n=0)
//  T = 0 on the back border (n=N-1)
//  Linear decrease from 1 to 0 on the upper (m=0)
//  and lower (m=M-1) borders:
//
//  The stationary solution is, obviously, a linear
//  temperature gradient along the x direction where
//  each row has the same temperature dependence as
//  the upper and lower borders.
//
//  The initial condition is the exact solution plus a
//  random value at each grid point. The iterative
//  method must damp the initial random fluctiation and 
//  bring the temperature distribution to a linear gradient.
//
//  In this problem, two matrices U and V are needed. At
//  each iteration, V[m][n] are computed from the neighbor
//  values of U. Then the matrices are swapped (by swapping
//  pointers) so thet U is again the new value of the
//  approximate solution.
// ----------------------------------------------------
//
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
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
   int apagar=0;
   TR.Start(); 
   do
      {
         apagar++;
      error_norm = 0.0;
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
      cout << "cicles " << apagar << endl;
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

    

// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
/* This is file DSor.C (diagonal SOR)
 *
 * Solving Posson equation in 2 dimensions, using the
 * SOR (Gauss-Seidel + overrelaxation) method.
 *
 * Sequential code. The 2D Matrix is swapped along the
 * diagonals, in preparation of a general method for
 * handling the data dependencies that prevent immediate
 * parallelization.
 *
 * Lower performance with respect to the standard Sor.C
 * should be observed for large systems because, given the
 * way the 2x2 matrix is allocated, the diagonal swap does
 * not make an optimal usage os L2 cache reusage.  
 * ===================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <CpuTimer.h>

using namespace std;

// ------------------------------------
// Declaration of auxiliary functions
// used by the code
// ------------------------------------

void   InputData();
void   InitJob(int MM, int NN);
void   ExitJob();
void   PrintResult(int step);
void   PrintData();

// Internal data 
// *************

double **U, **F;
double anorm, anormf, delta;
double EPS = 1.0e-5;
double PI = 3.1415926535;
int    nIter;             /* iteration counter */

// Data read from file "sor.dat" 
// *****************************

int N, M, Lgth; 
int maxIts, nTh;
int stepReport;
double Lx, Ly; 
double omega;

/* ---------------------------------------------------------
 * This is a global function, called by main(). It will
 * be transformed into a task function in parallel versions
 * of this code.
 * ---------------------------------------------------------*/
#ifdef DIAGONAL_SWAP

void SwapFct()
   {
   int n, m, k, rank;
   double resid, error_norm;

   do
      {
      error_norm = 0.0;
      anorm = 0.0;

      // --------------------------------------------------
      // This for loop swaps diagonals starting from lower 
      // left vertex, and including the main diagional
      // --------------------------------------------------
      for(n=1; n<(N-1); n++)
         {
         k = n;
         for(m=1; m<=n; m++)
            {
            resid = U[m+1][k] + U[m-1][k] + U[m][k-1] + U[m][k+1]
                              - 4 * U[m][k] - F[m][k];
            error_norm += fabs(resid);
            U[m][k] += 0.25 * omega * resid; 
            k--;   
            }
         }
      
      // ---------------------------------------------------------
      // This swaps the remaining diagonals to the right of the
      // main diagonal
      // ---------------------------------------------------------
      for(m=2; m<(M-1); m++)
         {
         k = m;
         for(n=N-2; n>=m; n--)
            {
            resid = U[k+1][n] + U[k-1][n] + U[k][n-1] 
                    + U[k][n+1] - 4 * U[k][n] - F[k][n];
            error_norm += fabs(resid);
            U[k][n] += 0.25 * omega * resid;    
            k++;
            }
         }
     
      nIter++; 
      if(nIter%stepReport==0) printf("\n Iteration %d done", nIter);
      }while( (error_norm > EPS*anormf) && (nIter <= maxIts) );
   }

#else

void SwapFct()
   {
   int n, m;
   double resid, error_norm;

   do
      {
      error_norm = 0.0;
      for(m=1; m<(M-1); m++)
         {
         for(n=1; n<(N-1); n++)
            {
            resid = U[m+1][n] + U[m-1][n] + U[m][n-1] + U[m][n+1]
                              - 4 * U[m][n] - F[m][n];
            error_norm += fabs(resid);
            U[m][n] += 0.25 * omega * resid; 
            }
         }
      nIter++; 
      if(nIter%stepReport==0) printf("\n Iteration %d done", nIter);
      }while( (error_norm > EPS*anormf) && (nIter <= maxIts) );
   }


#endif


int main(int argc, char **argv)
   {
   int n, status;
   CpuTimer Trep;

   InputData();
   InitJob(M, N);

   //omega = 2.0 / (1.0 + PI/N);
   omega = 1.9;

   Trep.Start();
   SwapFct();
   Trep.Stop();

   printf("\n\n Data: N = %d  M = %d   maxIts = %d\n", N, M, maxIts);
   if(nIter >maxIts) printf("\n Maximal number of iterations exeeded\n");
   else 
      {
      printf("\n Initial error  : %g", anormf);
      printf("\n Final error    : %g", anorm);
      printf("\n Iterations     : %d", nIter);
      printf("\n Active threads : %d\n", nTh);
      Trep.Report();
      }

   ExitJob();
   return 0;
   }


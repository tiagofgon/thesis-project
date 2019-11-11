// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
//
// This is file DSorb.C (diagonal Sor)
//
// Sequential code, like DSor.C. The only difference is that we have
// introduced an auxiliary function to swap a matrix diagonal, in
// preparation of the parallel versions of this code. It is the
// SwapDiagonal() function that will be made a parallel routine,
// using TBB parallel_reduce
//
// This sequential code is roughly 4 times slower than the Sor.C
// sequential version. An educated guess is that this is due to a
// very bad reusage of the L2 cache. Given the way the matrix is
// allocated, the traditional swap along rows makes a better usage
// of the L2 cache, since one is accessing successive array elements.
// This is not the case with the diagonal swap.
// -------------------------------------------------------------
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
double error_norm;

// Data read from file "sor.dat" 
// *****************************
int N, M, Lgth; 
int maxIts, nTh;
int stepReport;
double Lx, Ly; 
double omega;


double SwapDiagonal(int mi, int ni)
   {
   // ---------------------------------------------------------
   // In this function, [mi, ni] is the upper starting point of
   // the diagonal, and "size" the number of downward steps
   // At each step, mi is decreased and ni is increased.
   // ---------------------------------------------------------
   int m, n;
   double resid;
   double diag_error;

   diag_error = 0.0;
   m = mi;
   n = ni;
   do
      {
      resid = U[m+1][n] + U[m-1][n] + U[m][n-1] 
              + U[m][n+1] - 4 * U[m][n] - F[m][n];
      diag_error += fabs(resid);
      U[m][n] += 0.25 * omega * resid;
      m--; 
      n++; 
      }while( (m>0) && (n<(N-1)));
   return diag_error;
   }
    
double SwapDiagonalBis(int m, int n, int mode)
   {
   // ---------------------------------------------------------
   // In this function, [mi, ni] is the upper starting point of
   // the diagonal, and "size" the number of downward steps
   // At each step, mi is decreased and ni is increased.
   // The "mode" argument informs if we are swapping the lower
   // part of the matrix, including the main diagonal, or the
   // upperpart.
   // ---------------------------------------------------------
   double resid;
   double diag_error;
   int beg, end;

   diag_error = 0.0;
   beg = 0;
   if(mode==0) end = m;
   else end = N-n-1;

   for(int k=beg; k<end; ++k)
      {
      resid = U[m-k+1][n+k] + U[m-k-1][n+k] + U[m-k][n+k-1] 
              + U[m-k][n+k+1] - 4 * U[m-k][n+k] - F[m-k][n+k];
      diag_error += fabs(resid);
      U[m-k][n+k] += 0.25 * omega * resid; 
      }
   return diag_error;
   }    


/* -----------------------------------------------
 * The global function that drives the computation 
 * ----------------------------------------------*/
void SwapFct()
   {
   int n, m, k, rank;

   do
      {
      error_norm = 0.0;
      // ---------------------------------------------------------
      // This swaps diagonals starting from upper left vertex, and
      // including the main diagional
      // ---------------------------------------------------------
      for(m=1; m<M-1; m++) error_norm += SwapDiagonalBis(m, 1, 0);
      // ---------------------------------------------------------
      // This swaps the remaining diagonals to the right of the
      // main diagonal
      // ---------------------------------------------------------
      for(n=2; n<(N-1); n++) error_norm +=SwapDiagonalBis(M-2, n, 1); 
      nIter++; 
      if(nIter%stepReport==0) 
          printf("\n Iteration %d done", nIter);
      }while( (error_norm > EPS*anormf) && (nIter <= maxIts) );
   }

// The main() function
// -------------------
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
      printf("\n Final error    : %g", error_norm);
      printf("\n Iterations     : %d", nIter);
      printf("\n Active threads : %d\n", nTh);
      Trep.Report();
      }

   ExitJob();
   return 0;
   }

//////////////////////////////////////////////////////

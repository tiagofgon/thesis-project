// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// This is file SOR.C
// ******************
//
// Version 1: basic sequential code.
// ------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <CpuTimer.h>

// ------------------------------------
// Declaration of auxiliary functions
// used by the code
// ------------------------------------

void   InputData();
void   InitJob(int MM, int NN);
void   ExitJob();
void   PrintResult(int step);

// ---------------------
// Internal global data 
// ---------------------
double **U, **F;
double anorm, anormf, delta, omega;
const double EPS = 1.0e-5;
const double PI  = 3.1415926535;
int    nIter;             // iteration counter 

// -----------------------------
// Data read from file "sor.dat"
// -----------------------------
int N, M, Lgth;
int maxIts;
int nTh;
int stepReport;

main(int argc, char **argv)
   {
   int n, m; 
   int mSh, nSh, pass, status;
   int offset;
   double resid, error_norm;

   InputData();
   InitJob(M, N);
   CpuTimer TR;

   nIter = 0;
   //omega = 1.9;
   omega = 2.0 / (1.0 + PI/N);

   TR.Start(); 
   do
      {
      error_norm = 0.0;
      anorm = 0.0;

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
     
      anorm = error_norm;
      nIter++;
      if(nIter%stepReport==0) 
          printf("\n Iteration %d done", nIter);
      }while( (anorm > EPS*anormf) && (nIter <= maxIts) );
   TR.Stop(); 
 	  
   if(nIter >maxIts) 
      {
      printf("\n Maximal number of iterations exeeded\n");
      printf("\n Final error   : %g", anorm);
      }
   else 
      {
      printf("\n Initial error : %g", anormf);
      printf("\n Final error   : %g", anorm);
      printf("\n Iterations    : %d\n", nIter);
      }
   TR.Report();
   PrintResult(10);
   ExitJob();
   }


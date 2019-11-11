// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// This is file SORF2.C
// *********************
// Modify SorF.C, by unrolling the "pass" loop so that the
// collapse clause can now be used. This does not improve
// performance
// ------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <CpuTimer.h>
#include <omp.h>

// ------------------------------------
// Declaration of auxiliary functions
// used by the code
// ------------------------------------

void   InputData();
void   InitJob(int MM, int NN);
void   ExitJob();
void   PrintResult(int step);
void   PrintData();

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
int stepReport;
int nTh;

main(int argc, char **argv)
   {
   int n, m; 
   int mSh, nSh, pass, status;
   int offset;
   double resid;

   InputData();
   InitJob(M, N);
   CpuTimer TR;

   if(argc==2) nTh = atoi(argv[1]);
   nIter = 0;
   omega = 1.8;
   omp_set_num_threads(nTh);

   TR.Start(); 
   do
      {
      anorm = 0.0;
          #pragma omp parallel for reduction(+:anorm)
          for(m=1; m<(M-1); m++)
             {
             nSh = m%2;
             for(n=nSh+1; n<(N-1); n+=2)
                {
                resid = U[m+1][n] + U[m-1][n] + U[m][n-1] + U[m][n+1]
	 	               - 4 * U[m][n] - F[m][n];
	        anorm += fabs(resid);
                U[m][n] += 0.25 * omega * resid;
	        }
	     }
          mSh = pass/2;
          #pragma omp parallel for reduction(+:anorm)
          for(m=1; m<(M-1); m++)
             {
             nSh = (m+1)%2;
             for(n=nSh+1; n<(N-1); n+=2)
                {
                resid = U[m+1][n] + U[m-1][n] + U[m][n-1] + U[m][n+1]
	 	               - 4 * U[m][n] - F[m][n];
	        anorm += fabs(resid);
                U[m][n] += 0.25 * omega * resid;
	        }
	     }
      nIter++;
      if(nIter%stepReport==0) printf("\n Iteration %d done", nIter);
      }while( (anorm > EPS*anormf) && (nIter <= maxIts) );

   TR.Stop(); 
 	  
   PrintData();
   PrintResult(20);
   TR.Report();
   ExitJob();
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


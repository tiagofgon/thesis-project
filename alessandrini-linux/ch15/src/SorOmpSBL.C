// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// This is file SorOmpSBL.C
//
// ------------------------------------------------------------
// REMINDER: pipeline classes are:
//
// PipeBL<T>, Boolean lock synchronization, T being the BLock
//            type selected (BLock, SpBLock, OBlock)
//
// PipeThQ<T> ThQueue synchronization, T being the type of the
//            token flowing alo,g the pipeline
// ------------------------------------------------------------
//
// THIS CODE: OpenMP threads, PipeBL with spin  SpBLock
// ------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <CpuTimer.h>
#include <omp.h>
#include <Reduction.h>
#include <ThreadRangeOmp.h>
#include <PipeBL.h>
#include <SpBLock.h>

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

Reduction<double>  R;
PipeBL<SpBLock>   *P;

// -----------------------------
// Data read from file "sor.dat"
// -----------------------------
int N, M, Lgth;
int maxIts;
int nTh;
int stepReport;

void ThreadFct()
   {
   int n, m, rank;
   int nL, nH;
   int status;
   double resid, error_norm;

   rank = omp_get_thread_num();
   nL = 1;
   nH = N-1;
   ThreadRangeOmp(nL, nH);

   error_norm = 0.0;
   for(m=1; m<(M-1); m++)
      {
      P->WaitForRelease(rank);
      for(n=nL; n<nH; n++)
         {
         resid = U[m+1][n] + U[m-1][n] + U[m][n-1] + U[m][n+1]
                           - 4 * U[m][n] - F[m][n];
         error_norm += fabs(resid);
         U[m][n] += 0.25 * omega * resid;    
         }
      P->ReleaseNext(rank);
      }
   R.Accumulate(error_norm);
   }


main(int argc, char **argv)
   {
   InputData();
   InitJob(M, N);
   CpuTimer TR;

   if(argc==2) nTh = atoi(argv[1]);

   P = new PipeBL<SpBLock>(0, nTh-1);

   omp_set_num_threads(nTh);
   nIter = 0;
   //omega = 1.9;
   omega = 2.0 / (1.0 + PI/N);

   TR.Start(); 
   do
      {
      #pragma omp parallel
         { ThreadFct(); }
      anorm = R.Data();
      R.Reset();
      nIter++;
      if(nIter%stepReport==0) 
          {
          printf("\n Iteradion %d done", nIter);
          printf("\n Current error %g \n", anorm);
          }
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


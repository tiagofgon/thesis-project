// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// This is file SorOmp.C
//
// OpenMP macrotasking version. Very close to the other
// macrotasking verion (using NPool). The task function
// is practically the same
// ====================================================

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <ThreadRangeOmp.h>
#include <CpuTimer.h>

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
double anorm, anormf, global_error, delta, omega;
const double EPS = 1.0e-5;
const double PI  = 3.1415926535;
int    nIter;             // iteration counter 

// -----------------------------
// Data read from file "sor.dat"
// -----------------------------
int N, M, Lgth;
int maxIts, nTh, stepReport;

// ------------------------
// The OpenMP task function. 
// ------------------------
void TaskFct()
   {
   int n, m;
   int beg, end;
   int mSh, nSh, pass;
   double resid, error_norm;

   // Compute the thread range
   // ------------------------
   beg = 1;
   end = M-1;       // end of global range
   ThreadRangeOmp(beg, end);

   do
      {
      error_norm = 0.0;
      for(pass=1; pass <=2; pass++)   // odd-even ordering
         {
         mSh = pass/2;
         for(m=beg; m<end; m++)
            {
            nSh = (m+mSh)%2;
            for(n=nSh+1; n<(N-1); n+=2)
               {
               resid = U[m+1][n] + U[m-1][n] + U[m][n-1] + U[m][n+1]
		               - 4 * U[m][n] - F[m][n];
	       error_norm += fabs(resid);
               U[m][n] += 0.25 * omega * resid;    
	       }
	    }
		 
         if(pass==2)
            {
            #pragma omp critical
               { global_error += error_norm; }
            }

         #pragma omp barrier  
	 // This is the end of a half (even or odd) swap.
         }

      // Enter sequential region
      // ----------------------- 
      #pragma omp master
	  {
	  anorm = global_error;
	  global_error = 0.0;
	  nIter++;
          if(nIter%stepReport==0) printf("\n Iteradion %d done", nIter);
	  } 
      #pragma omp barrier
      }while( (anorm > EPS*anormf) && (nIter <= maxIts) );
   }


int main(int argc, char **argv)
   {
   int n, status;

   omega = 1.8;
   InputData();
   InitJob(M, N);
   CpuTimer TR;

   if(argc==2) nTh=atoi(argv[1]);
   omp_set_num_threads(nTh); 
   TR.Start(); 
   #pragma omp parallel 
      {
      TaskFct();
      }
   TR.Stop();

   PrintData();
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


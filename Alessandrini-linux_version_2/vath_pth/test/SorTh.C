// **************************************
// Copyright (c) 2011 Victor Alessandrini
// All rights reserved.
// **************************************
/* This is file SORTH.C
 * 
 * Implements Successve Overrelaxation with Tchebichef 
 * accelleration for solving a two dimensional boundary
 * value problem.
 * 
 * Domain has NxM grid. Horizontal dimension is "Lgth" 
 * in x direction, therefore delta=Lgth/(N-1). The same
 * value of delta acts in y direction. If M!=N, physical
 * dimensions are not the same.
 *
 * Charge distribution is sin(x*PI)*sin(x*PI)*
 * sin(y*PI*N/M)*sin(y*PI*N/M), where x = delta*(n-1) 
 * and y = delta*(m-1). This vanishes on the sides of 
 * the problem domain.
 * THIS IS THE FINAL VERSION OF THE CODE.
 * =====================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SPool.h>
#include <Reduction.h>
#include <CpuTimer.h>

#ifdef SPIN_BARRIER
#include <SpBarrier.h>
#else
#include <Barrier.h>
#endif

// ------------------------------------
// Declaration of auxiliary functions
// used by the code
// ------------------------------------

void   InputData();
void   InitJob(int MM, int NN, int nThreads);
void   ExitJob();
double **AllocMatrix(int MM, int NN);
void   FreeMatrix(double **d);
void   PrintMatrix(double **d, int NN, int MM);
double SetInitialValues();
void   err_exit(char *p);
void   Timer_Report();

// ---------------------
// Internal global data 
// ---------------------

double **U, **F;
double anorm, anormf, rJac, delta;
const double EPS = 1.0e-5;
const double PI  = 3.1415926535;
int    nIter;             // iteration counter 

// -------------------------------------
// Global data related to multithreading 
// --------------------------------------

SPool            *TP;        // thread pool
Reduction<double> RD;        // To perform the reduction of the global error
#ifdef SPIN_BARRIER
SpBarrier        *barrier;     // spin barrier
#else
Barrier          *barrier;     // barrier
#endif

// -----------------------------
// Data read from file "sor.dat"
// -----------------------------

int N, M, Lgth;
int maxIts, nTh;

/* --------------------------------------------
 * The task function. We adopt the ThPeer
 * signature: the function receives a (void *)
 * and returns nothing.
 * We pass as argument a Range<int> object that
 * describes the integer range where the work
 * is partitioned.
 * -------------------------------------------*/

void TaskFct(void *p)
   {
   int n, m, rank;
   int beg, end;
   int mSh, nSh, pass, status;
   double resid, error_norm, omega;

   rank = TP->GetRank();
   beg = 2;       // start of global range
   end = M;       // end of global range

   // Compute the thread range from the global range
   // ----------------------------------------------
   //printf("\n Thread %d,  global range %d   %d", rank, beg, end);
   TP->ThreadRange(beg, end);
   printf("\n Thread %d,  thread range %d   %d\n", rank, beg, end);

   omega = 1.0;   // each thread has its own copy of the SOR
		  // parameter. Optimizes synchronizations. 
   do
      {
      error_norm = 0.0;
      for(pass=1; pass <=2; pass++)   // odd-even ordering
         {
         mSh = pass/2;
         for(m=beg; m<end; m++)
            {
            nSh = (m+mSh)%2 + 1;
            for(n=nSh+1; n<N; n+=2)
               {
               resid = U[m+1][n] + U[m-1][n] + U[m][n-1] + U[m][n+1]
		               - 4 * U[m][n] - F[m][n];
	       error_norm += fabs(resid);
               U[m][n] += 0.25 * omega * resid;    
	       }
	    }
		 
        // --------------------------------------------
	// Redefine omega, and cumulate global error, but
	// only at the end of the second pass.
	// -------------------------------------------- 
		 
         omega = ( nIter==1 && pass==1 ?  1.0/(1.0 - 0.5 *rJac*rJac) :
				   1.0/(1.0 - 0.25*rJac*rJac*omega) );

         if(pass==2) RD.Accumulate(error_norm);
         status = barrier->Wait();  
	 // This is the end of a half (even or odd) swap.
         }

      // -----------------------
      // Enter sequential region
      // ----------------------- 

      if(rank==1)
	  {
	  anorm = RD.Data();
	  RD.Reset();
	  nIter++;
          if(nIter%100==0) printf("\n Iteradion %d done", nIter);
	  } 
      status = barrier->Wait();
      // Exit sequential region 
      }while( (anorm > EPS*anormf) && (nIter <= maxIts) );
   }


int main(int argc, char **argv)
   {
   int n, status;

   InputData();
   InitJob(M, N, nTh);
   CpuTimer TR;

   std::cout << "\n Solving 2D Poisson equation " << std::endl;
   // ---------------------
   // Thread pool operation
   // ---------------------
   TR.Start(); 
   TP->Dispatch(TaskFct, NULL);
   TP->WaitForIdle();
   TR.Stop();

   // -------------
   // Output result
   // -------------
   printf("\n\n Data: N = %d  M = %d   maxIts = %d\n", N, M, maxIts);
   if(nIter >maxIts) printf("\n Maximal number of iterations exceeded\n");
   else 
      {
      printf("\n Initial error  : %g", anormf);
      printf("\n Final error    : %g", anorm);
      printf("\n Iterations     : %d", nIter);
      printf("\n Active threads : %d\n", nTh);
      TR.Report();
      }
   ExitJob();
   }


/***********************************
 * Definition of Auxiliary Functions
 ***********************************/

/* -------------------------------------------------
 * This function reads from file "sor.dat" the sizes
 * N, M of the problem and the maximal number of
 * iterations, maxIts.
 * Uses traditional C I/O. 
 * ------------------------------------------------*/
void InputData()
    {
    FILE *fp;
    char buffer[128];

    if( !(fp = fopen("sor.dat", "r") ))
	   {
	   printf("\n Input error\n");
	   exit(0);
	   }
	
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &N);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &M);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &Lgth);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &maxIts);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &nTh);
    fclose(fp);
    }
   
void InitJob(int MM, int NN, int nThreads)
   {
   int n;	   
   U = AllocMatrix(MM, NN);
   F = AllocMatrix(MM, NN);

   #ifdef SPIN_BARRIER
   barrier =  new SpBarrier(nThreads);
   #else
   barrier =  new Barrier(nThreads);
   #endif

   TP = new SPool(nThreads);
   
   delta = (double)Lgth/(NN-1);
   anormf = SetInitialValues();
   rJac = 0.5 * (cos(PI/NN) + cos(PI/MM));
   nIter = 0;
   }

void ExitJob()
   {
   delete barrier;
   delete TP;
   FreeMatrix(U);
   FreeMatrix(F);
   }


/* ---------------------------------------------------
 * This function sets the boundary and initial values
 * of the U fiels, and returns the norm of the error of
 * the initial configuration.
 * 
 * Initial boundary conditions correspond to U = 0 on
 * the borders. The charge density configuration is 
 * sin(x*PI)*sin(x*PI)* sin(y*PI*N/M)*sin(y*PI*N/M), 
 * where x = delta*(n-1) * and y = delta*(m-1). This 
 * vanishes on the borders of the problem domain.
 * ------------------------------------------------*/

double SetInitialValues()
   {
   int n, m;
   double error, x, y;
   
   /* Set the U initial values */
   
   for(m=1; m<=M; m++)
	  for(n=1; n<=N; n++) U[m][n] = 0.0;
   
   /* Set F[m][n] = delta*delta*rho(m, n) */

   for(m=2; m<M; m++)
      {
      y = delta * (m-1);
      for(n=2; n<N; n++)
         {
         x = delta * (n-1);
         error = sin(x*PI) * sin(y*PI*N/M);
	 F[m][n] = delta * delta * error * error;
	 }
      }

   /* Compute initial error */

   error = 0.0;
   for(m=2; m<M; m++)
       for(n=2; n<N; n++) error += fabs(F[m][n]);
   return error;
   }
    
/*------------------------------ 
 * Memory management functions 
 *-----------------------------*/

double **AllocMatrix(int M, int N)
    {
    int n;
    double **d;
	
    d = (double **)malloc( (M+1)*sizeof(double *) );
    if(!d)
	{	
	printf("\n malloc failed for d\n");
	exit(0);
	}
    d[1] = (double *)malloc ( (M*N+1)*sizeof(double) );
    if(!d[1]) 
	{
	printf("\n malloc failed for d[1]\n");
	exit(0);
	}
    for(n=2; n<=M; n++) d[n] = d[n-1] + N;
    return d;
    }

void FreeMatrix(double **d)
    {
    free(d[1]);
    free(d);
    }
			 
/*-------------------
 * Printing matrices
 *-------------------*/

void PrintMatrix(double **d, int NN, int MM)
   {
   int n, m;

   printf("\n\n");
   for(m=1; m<=MM; m++)
       {
       for(n=1; n<=NN; n++)
	      {
          printf(" %g   ", d[m][n]);
          if(n%5==0) printf("\n");
          }
       printf("\n\n");
       }
   }

void err_exit(char *p)
   {
     printf("\n %s ", p);
     exit(0);
   }
/********************************************************/

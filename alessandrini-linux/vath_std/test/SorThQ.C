// **************************************
// Copyright (c) 2012 Victor Alessandrini
// All rights reserved.
// **************************************
// **************************************
// Copyright (c) 2011 Victor Alessandrini
// All rights reserved.
// **************************************
/* This is file Sor2Th.C
 * (2) means pipeline with ThQueue<int> connector.
 * ThSet environment
 *
 * THIS CODE WORKS CORRECTLY
 * ===================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Barrier.h>
#include <SPool.h>
#include <CpuTimer.h>
#include <PipeThQ.h>
#include <pthread.h>

using namespace std;

// Auxiliary functions 
// *******************

void   InputData();
void   InitJob(int MM, int NN);
void   ExitJob();
double **AllocMatrix(int MM, int NN);
void   FreeMatrix(double **d);
void   PrintMatrix(double **d, int NN, int MM);
double SetInitialValues();
void   SetIndices(int I, int F, int nThreads, int rank, int *nl, int *nh);
void   err_exit(char *p);

// Internal data 
// *************

double **U, **F;
double anorm, anormf, delta;
double EPS = 1.0e-5;
double PI = 3.1415926535;
int    nIter;             /* iteration counter */

// Data related to multithreading 
// ******************************

SPool   *TS;
Barrier *B;
PipeThQ<int> *P;

#ifdef BOOST_ENV
boost::mutex emutex
#else
pthread_mutex_t emutex = PTHREAD_MUTEX_INITIALIZER;
#endif

// Data read from file "sor.dat" 
// *****************************

int N, M, Length_x; 
int maxIts, nTh;
double Lx, Ly; 
double omega;

// Timer data
// ********** 
CpuTimer TimeRep;

/* --------------------------------------------
 * The thread function. We adopt the Pthreads
 * signature: the function receives a (void *)
 * and returns a NULL void pointer.
 * -------------------------------------------*/

void ThreadFct(void *p)
   {
   int n, m, col, rank;
   bool flag;
   int nL, nH;  // lowest and highest columns 
   int status;
   double resid, error_norm, anormTest;

   rank = TS->GetRank();
   nL = 1;
   nH = M-1;
   TS->ThreadRange(nL, nH); 
   printf("\n Thread %d started", rank); 
   do
      {
      error_norm = 0.0;
      anorm = 0.0;

      if(rank==1)    // first stage in pipeline
         {
         for(m=1; m<(M-1); m++)
            {
            for(n=nL; n<nH; n++)
               {
               resid = U[m+1][n] + U[m-1][n] + U[m][n-1] + U[m][n+1]
                                 - 4 * U[m][n] - F[m][n];
               error_norm += fabs(resid);
               U[m][n] += 0.25 * omega * resid;    
               }
            P->PostNext(m, rank);
            }
         col = -1;
         P->PostNext(col, rank);   // signal end of iteration
         }
      else         // next stages in pipeline
         {
         col = P->GetPrevious(rank, flag);
         while(col>0)
            {
            for(n=nL; n<=nH; n++)
               {
               resid = U[col+1][n] + U[col-1][n] + U[col][n-1] + U[col][n+1]
                              - 4 * U[col][n] - F[col][n];
               error_norm += fabs(resid);
               U[col][n] += 0.25 * omega * resid;    
               }
            P->PostNext(col, rank);
            col = P->GetPrevious(rank, flag);
            }
         P->PostNext(col, rank);  // sifnal end of iteration
         }
      
      // perform a reduction of the error 
      // --------------------------------
      pthread_mutex_lock(&emutex);
      anorm += error_norm;
      if(rank==1) nIter++;
      pthread_mutex_unlock(&emutex);

      if(nTh>1) B->Wait(); 

      anormTest = anorm;
      if(nIter%200==0 && rank==1) printf("\n Iteradion %d done", nIter);
      if(nTh>1) B->Wait();
      }while( (anormTest > EPS*anormf) && (nIter <= maxIts) );

   if(rank==1) P->ClosePipeline();
   }


int main(int argc, char **argv)
   {
   int n, status;
    
   InputData();
   InitJob(M, N);

   omega = 2.0 / (1.0 + PI/N);

   // ------------------------------------
   // Create the  thread set, the pipeline 
   // and the barrier
   // -------------------------------------
   TS = new SPool(nTh);
   P = new PipeThQ<int>(1, nTh);  // ranks of threads in pipeline
   B = new Barrier(nTh);            //  nTh members of the gang
   std::cout << "\n Poisson equation: pipeline with queue synchronization\n" 
             << std::endl;

   // ------------------
   // Run the thread set
   // ------------------
   TimeRep.Start();
   TS->Dispatch(ThreadFct, NULL);
   TS->WaitForIdle();
   TimeRep.Stop();

   printf("\n\n Data: N = %d  M = %d   maxIts = %d\n", N, M, maxIts);
   if(nIter >maxIts) printf("\n Maximal number of iterations exeeded\n");
   else 
      {
      printf("\n Initial error  : %g", anormf);
      printf("\n Final error    : %g", anorm);
      printf("\n Iterations     : %d", nIter);
      printf("\n Active threads : %d\n", nTh);
      TimeRep.Report();
      }

   ExitJob();
   return 0;
   }


/************************
 * Auxiliary Functions
 ************************/

/* -------------------------------------------------
 * This function reads from file "sor.dat" the sizes
 * N, M of the problem and the maxumal number of
 * iterations, maxIts 
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
    sscanf(buffer, "%d", &Length_x);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &maxIts);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &nTh);
    fclose(fp);
    }
   
void InitJob(int MM, int NN)
   {
   int n;	   
   U = AllocMatrix(MM, NN);
   F = AllocMatrix(MM, NN);
   Lx = (double) Length_x;
   Ly = (Lx * (MM-1))/(NN-1);
   delta = (double)Length_x/(NN-1);
   anormf = SetInitialValues();
   nIter = 0;
   }

void ExitJob()
   {
   delete B;
   delete P;
   delete TS;
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
   double error, x, y, dx, dy;
   
   /* Set the U initial values */
   
   for(m=0; m<M; m++)
	  for(n=0; n<N; n++) U[m][n] = 0.0;
   
   /* Set F[m][n] = delta*delta*rho(m, n) */

   dx = 1.0/(N-1);
   dy = 1.0/(M-1);
   for(m=1; m<(M-1); m++)
      {
      y = m*dy;                  // CHECK
      for(n=1; n<(N-1); n++)
         {
         x = n*dx;              // CHECK
         error = sin(x*PI) * sin(y*PI);
         F[m][n] = delta * delta * error * error;
         }
      }

   /* Compute initial error */

   error = 0.0;
   for(m=1; m<(M-1); m++)
       for(n=1; n<(N-1); n++) error += fabs(F[m][n]);
   return error;
   }

// ----------------------------------------------------------
// Domain decomposition issues. The function below works as 
// follows:
//
// input I : first index value in global domain
// input F : last index value in global domain
// input nTh: the number of threads in the pool
// input rank : the rank of the calling thread. Ranks have the
//              same range tna in OpenMP: [0, nTh-1].
//
// output nl : first index value in local domain
// output nh : last index value in loca, dimaon
// ---------------------------------------------------------
    
void SetIndices(int I, int F, int nTh, int rank, int *nl, int *nh)
   {
   int n, nBase, nRest, index, size, nnl, nnh;
   
   size = F-I+1;
   nBase = size/nTh;
   nRest = size%nTh;
   index = I-1;
   n = 0;
   
   while(n<=rank)
      {
      index++;
      nnl = index;
      index += (nBase-1);
      if(nRest)
         {
         index++;
         nRest--;
         }
      nnh = index;
      n++;
      }
   *nl = nnl;
   *nh = nnh;
   }

 
/*------------------------------ 
 * Memory management functions 
 *-----------------------------*/

double **AllocMatrix(int M, int N)
    {
    int n;
    double **d;
	
    d = (double **)malloc( M * sizeof(double *) );
    if(!d)
	{	
	printf("\n malloc failed for d\n");
	exit(0);
	}
    d[0] = (double *)malloc ( M*N*sizeof(double) );
    if(!d[0]) 
	{
	printf("\n malloc failed for d[0]\n");
	exit(0);
	}
    for(n=1; n<M; n++) d[n] = d[n-1] + N;
    return d;
    }

void FreeMatrix(double **d)
    {
    free(d[0]);
    free(d);
    }
			 
/*-------------------
 * Printing matrices
 *-------------------*/
void PrintMatrix(double **d, int NN, int MM)
   {
   int n, m;

   printf("\n\n");
   for(m=0; m<MM; m++)
       {
       for(n=0; n<NN; n++)
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
//
//Comment on the deadlock bug
//****************************
//
// What actually happpened is that the threads were not well
// synchronized in the absence of the second barrier. The first
// barrier guarantees that all the threads have contributed to
// the reduction operation of the error, cumulated in "anorm".
// In principle, all the threads perform the end of the DO loop
// check with the same value of "anorm". But it can happen sometimes
// that the main thread rushes ahead and resets "anorm" to 0 for the
// next iteration, before some of the other threads emerging from
// the barrier has performed the check. In this case, this thread
// perform the check with a null error and breaks away from the
// DO loop. This means that the pipelina has lost an stage and
// at some point nobody moves data. This is why the pipeline 
// stalls
//
// In the new version, with the second barrier, there is absolutely
// no doubt that all threads perform the same test in all 
// circunstances.
//
// This code performs correctly even when the coeurs are overcommited
// namely nTh > (number of coeurs) which, for a pipelined code, is
// a verification of its robustness.
//
// ************************************************************

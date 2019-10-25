// **************************************
// Copyright (c) 2012 Victor Alessandrini
// All rights reserved.
// **************************************
// File HeaTOmp1.C
// ---------------
// Very simple problem : computation of temperatute
// distribution in 2-dimensional square domain with
// simple boundary conditions.
// 2D array of M rows and N columns
//
// Basic parallel version, using OpenMP,
//  - vectorized code, but different memory allocation 
//  - aligned memory allocation
//  - no strip mining
// -------------------------------------------
#define ALIGNED_ALLOCATION 1

#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <Rand.h>
#include <CpuTimer.h>
#include <omp.h>                 // new
#include <ThreadRangeOmp.h>                 // new

using namespace std;

// ----------------------------------
// Declaration of auxiliary functions
// ----------------------------------
void InputData();
void InitJob(int MM, int NN);
double SetInitialValues();
void ExitJob();
void PrintResult(int n);
double **AllocMatrix(int MM, int NN);
void FreeMatrix(double **d);
double **AllocAlignedMatrix(int MM, int NN);
void FreeAlignedMatrix(double **d);

// -----------------
// Global variables
// -----------------

double **U, **V;
const double EPS = 1.0e-5;
double initial_error, curr_error;
double error_cumul;          // new
int nIter;
const int cache_line = 8;   // number of doubles in Sandy 
                            // Bridge cache line

// ------------------------------
// Data read from file "heat.dat"
// ------------------------------
int N, M;
int maxIts;
int nThreads;                // new


// ------------------------
// Task function for OpenMP
// ------------------------
void TaskFunction()
   {
   int m, n;
   int beg, end;
   double thread_error, error;
   double **swap;

   // First, fix the thread loop range
   // --------------------------------
   beg = 1;
   end = M-1;
   ThreadRangeOmp(beg, end);   // [beg, end) becomes thread range

   do
      {
      double *UU = U[0];
      #ifdef ALIGNED_ALLOCATION
      __assume_aligned(UU, 64);
      #endif

      thread_error = 0.0;
      for(m=beg; m<end; m++)
            {
            #pragma simd
            for(n=1; n<(N-1); n++)
               {
               error = UU[(m+1)*N+n] + UU[(m-1)*N+n] + UU[m*N+(n-1)] + 
                       UU[m*N+(n+1)] - 4 * UU[m*N+n];
	       thread_error += fabs(error);
               V[m][n] = U[m][n] + 0.25 * error;    
	       }
	    }
      #pragma omp critical
        { error_cumul += thread_error; }

      #pragma omp barrier
     
      #pragma omp master
         {
         swap = U;    // swap pointers, so that U is 
         U = V;       // again the current solution
         V = swap;
         curr_error = error_cumul;
         error_cumul = 0;
         nIter++;
         if(nIter%10000==0) cout << "\n Iteration " << nIter << " done" << endl;
         }
      #pragma omp barrier
      }while( (curr_error > EPS*initial_error) && (nIter <= maxIts) );
   }


int main(int argc, char **argv)
   {
   CpuTimer TR;
   
   InputData();
   if(argc==2) nThreads = atoi(argv[1]);   // override file input
   
   // ---------------------------------------------------------
   // Here, we adjust the horizontal size N to make it an exact
   // multiple of the L2 cache line size.
   // ---------------------------------------------------------
   int subrange = N/nThreads;
   while(subrange%cache_line) subrange--;
   N = subrange*nThreads;
   std::cout << "\n Modified horizontal size is " << N << std::endl;

   InitJob(M, N);

   omp_set_num_threads(nThreads);
   TR.Start();
   error_cumul = 0.0;
   #pragma omp parallel 
      {
      TaskFunction();
      }
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


// ********************
// Auxiliary Functions
// *******************

/* -------------------------------------------------
 * This function reads from file "heat.dat" the sizes
 * N, M of the problem and the maximal number of
 * iterations, maxIts.
 * Uses traditional C I/O. 
 * ------------------------------------------------*/
void InputData()
    {
    FILE *fp;
    char buffer[128];

    if( !(fp = fopen("heat.dat", "r") ))
	   {
	   cout << "\n Input error" << endl;
	   exit(0);
	   }
	
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &N);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &M);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &maxIts);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &nThreads);
    fclose(fp);
    }
   
void InitJob(int MM, int NN)
   {
   int n;
   #ifdef ALIGNED_ALLOCATION	   
   U = AllocAlignedMatrix(MM, NN);
   V = AllocAlignedMatrix(MM, NN);
   #else
   U = AllocMatrix(MM, NN);
   V = AllocMatrix(MM, NN);
   #endif

   initial_error = SetInitialValues();
   nIter = 0;
   }

void ExitJob()
   {
   #ifdef ALIGNED_ALLOCATION
   FreeAlignedMatrix(U);
   FreeAlignedMatrix(V);
   #else
   FreeMatrix(U);
   FreeMatrix(V);
   #endif
   }


/* ---------------------------------------------------
 * This function sets the boundary and initial values
 * of the U fiels, and returns the norm of the error of
 * the initial configuration.
 * ------------------------------------------------*/

double SetInitialValues()
   {
   int n, m;
   double a, retval, residue;
   Rand R(999);
   
   // ------------------------------------------------
   // Set the U, V initial values  as well as
   // the boundary values on the borders of the domain.
   // First, boundary values
   // ------------------------------------------------
   
   for(m=0; m<M; m++)
      {
      U[m][0] = 1.0;
      V[m][0] = 1.0;
      U[m][N-1] = 0.0;
      V[m][N-1] = 0.0;
      }
 
   a = -1.0/(N-1);
   for(n=0; n<N; n++)
      {
      U[0][n] = a*n+1;
      V[0][n] = a*n+1;
      U[M-1][n] = a*n+1;
      V[M-1][n] = a*n+1;
      }
   
   // ---------------------------------------
   // Interior values : exact result + random
   // ---------------------------------------
   for(m=1; m<(M-1); m++)
      for(n=1; n<(N-1); n++) 
      {
      U[m][n] = a*n+1 + 0.0001*R.draw();
      V[m][n] = a*n+1 + 0.0001*R.draw();
      }

   // -------------------------
   // Compute the initial error
   // -------------------------
   retval = 0;
   for(m=1; m<(M-1); m++)
      for(n=1; n<(N-1); n++)  
         {
         residue = U[m+1][n] + U[m-1][n] + U[m][n-1] + U[m][n+1]
		             - 4 * U[m][n];
         retval += fabs(residue);
         }

   return retval;
   }
    

/*------------------------------ 
 * Memory management functions 
 * Allocations with offset 0.
 *-----------------------------*/

double **AllocMatrix(int M, int N)
    {
    int n;
    double **d;
	
    d = (double **)malloc( M*sizeof(double *) );
    if(!d)
	{	
	cout << "\n malloc failed for d" << endl;
	exit(0);
	}
    d[0] = (double *)malloc ( (M*N)*sizeof(double) );
    if(!d[0]) 
	{
	cout << "\n malloc failed for d[0]" << endl;
	exit(0);
	}
    for(n=1; n<M; n++) d[n] = d[n-1] + N;
    return d;
    }

double **AllocAlignedMatrix(int M, int N)
    {
    int n;
    double **d;
	
    d = (double **) _mm_malloc( M*sizeof(double *), 64);
    if(!d)
	{	
	cout << "\n _mm_malloc failed for d" << endl;
	exit(0);
	}
    d[0] = (double *) _mm_malloc ( (M*N)*sizeof(double), 64 );
    if(!d[0]) 
	{
	cout << "\n malloc failed for d[0]" << endl;
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

void FreeAlignedMatrix(double **d)
    {
    _mm_free(d[0]);
    _mm_ free(d);
    }

void PrintResult(int step)	
   {
   int n;
   int counter = 0;
 
   cout << "\n Solution profile for m = M/2 : \n\n";
   for(n=1; n<=(N-1); n+=step)
       {
       cout << U[M/2][n] << "    " ;
       counter++;
       if(counter%4 == 0) 
          {
          cout << endl;
          counter = 0;
          }
       }
   }

    

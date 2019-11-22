// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
//
// File HeatAux.cpp
//
// This module collects a numer of auxiliary functions used
// in all the implementations of the Heat code
// --------------------------------------------------------
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <Rand.hpp>

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

// ------------------------------------------
// Global variables, defined in other modules
// -----------------------------------------
extern double **U, **V;
const double EPS = 1.0e-5;
extern double initial_error, curr_error;
extern int nIter;

// ------------------------------
// Data read from file "heat.dat"
// ------------------------------
extern int N, M;
extern int maxIts;
extern int nThreads;
extern int stepReport;

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
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &stepReport);
    fclose(fp);
    }
   
void InitJob(int MM, int NN)
   {
   int n;	   
   U = AllocMatrix(MM, NN);
   V = AllocMatrix(MM, NN);
   initial_error = SetInitialValues();
   nIter = 0;
   }

void ExitJob()
   {
   FreeMatrix(U);
   FreeMatrix(V);
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

void FreeMatrix(double **d)
    {
    free(d[0]);
    free(d);
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

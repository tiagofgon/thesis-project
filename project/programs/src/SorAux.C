// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// This is file SorAux.C
//
// Common auxiliary utility functions for
// SOR codes
// --------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>

// ------------------------------------
// Declaration of auxiliary functions
// used by the code
// ------------------------------------

void   InputData();
void   InitJob(int MM, int NN);
void   ExitJob();
void   PrintResult(int step);
double **AllocMatrix(int MM, int NN);
void   FreeMatrix(double **d);
//void   PrintMatrix(double **d, int NN, int MM);
void   PrintResult(int step);
double SetInitialValues();

// ---------------------
// Internal global data 
// ---------------------
extern int N, M;
extern double **U, **F;
extern double anormf, delta;
extern int Lgth, stepReport;
extern int maxIts, nTh;
const double PI  = 3.1415926535;

// -------------------------------------------------
// This function reads from file "sor.dat" the sizes
// N, M of the problem and the maximal number of
// iterations, maxIts.
// Uses traditional C I/O. 
// ------------------------------------------------
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
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &stepReport);
    fclose(fp);
    }
   

void InitJob(int MM, int NN)
   {
   int n;	   
   U = AllocMatrix(MM, NN);
   F = AllocMatrix(MM, NN);
   delta = (double)Lgth/(NN-1);
   anormf = SetInitialValues();
   printf("\n Value of anormf is : %g", anormf);
   }

void ExitJob()
   {
   FreeMatrix(U);
   FreeMatrix(F);
   }


// ---------------------------------------------------
// This function sets the boundary and initial values
// of the U fiels, and returns the norm of the error of
// the initial configuration.
//
// Initial boundary conditions correspond to U = 0 on
// the borders. The charge density configuration is 
// sin(x*PI)*sin(x*PI)* sin(y*PI*N/M)*sin(y*PI*N/M), 
// where x = delta*(n-1) * and y = delta*(m-1). This 
// vanishes on the borders of the problem domain.
// -------------------------------------------------
double SetInitialValues()
   {
   int n, m;
   double error, x, y;
   
   // Set the U initial values
   // ------------------------
   for(m=0; m<M; m++)
	  for(n=0; n<N; n++) U[m][n] = 0.0;
   
   // Set F[m][n] = delta*delta*rho(m, n)
   // ----------------------------------- 
   for(m=1; m<(M-1); m++)
      {
      y = delta * (m-1);
      for(n=1; n<(N-1); n++)
         {
         x = delta * (n-1);
         error = sin(x*PI) * sin(y*PI*N/M);
	 F[m][n] = delta * delta * error * error;
	 }
      }

   // Compute initial error
   error = 0.0;
   for(m=1; m<(M-1); m++)
       for(n=1; n<(N-1); n++) error += fabs(F[m][n]);
   return error;
   }
    
// ---------------------------- 
// Memory management functions 
// ----------------------------
double **AllocMatrix(int M, int N)
    {
    int n;
    double **d;
	
    d = (double **)malloc( M*sizeof(double *) );
    if(!d)
	{	
	printf("\n malloc failed for d\n");
	exit(0);
	}
    d[0] = (double *)malloc ( (M*N)*sizeof(double) );
    if(!d[0]) 
	{
	printf("\n malloc failed for d[1]\n");
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
 
   std::cout << "\n Solution profile for m = M/2 : \n\n";
   for(n=0; n<(N-1); n+=step)
       {
       std::cout << U[M/2][n] << "    " ;
       counter++;
       if(counter%4 == 0) 
          {
          std::cout << std::endl;
          counter = 0;
          }
       }
   }


// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File HeatOmp.C
// 
// OpenMP macrotasking version. 
// -------------------------------------------

#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <Rand.hpp>
#include <CpuTimer.hpp>
#include <omp.h>                 // new
#include <ThRangeOmp.hpp>                 // new

using namespace std;

// ----------------------------------
// Declaration of auxiliary functions
// ----------------------------------
void InputData();
void InitJob(int MM, int NN);
void ExitJob();
void PrintResult(int n);

// -----------------
// Global variables
// -----------------

double **U, **V;
const double EPS = 1.0e-5;
double initial_error, curr_error;
double error_cumul;
int    nIter;
bool   moreCycles;
 
// ------------------------------
// Data read from file "heat.dat"
// ------------------------------
int N, M;
int maxIts;
int nThreads;                // new
int stepReport;

// ---------------------------------------------
// Auxiliary function to prepare next iteration,
// and decide if it must take place
// ---------------------------------------------
bool NextIteration(double error)
   {
   double **swap = U;   // swap pointers
   U = V;
   V = swap;
   nIter++;
   if(nIter%stepReport==0) cout << "\n Iteration " << nIter << endl;
   if( (error > EPS*initial_error) && (nIter <= maxIts) ) 
       {
       curr_error = error;
       error_cumul = 0.0;
       return true;
       }
   else return false;
   }

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
      thread_error = 0.0;
      for(m=beg; m<end; m++)
            {
            for(n=1; n<(N-1); n++)
               {
               error = U[m+1][n] + U[m-1][n] + U[m][n-1] + U[m][n+1]
		               - 4 * U[m][n];
	       thread_error += fabs(error);
               V[m][n] = U[m][n] + 0.25 * error;    
	       }
	    }
      #pragma omp critical
      { error_cumul += thread_error; }

      #pragma omp barrier
      #pragma omp master
         { moreCycles = NextIteration(error_cumul); }
      #pragma omp barrier
      }while( moreCycles);
   }


int main(int argc, char **argv)
   {
   InputData();
   InitJob(M, N);
   CpuTimer TR;

   if(argc==2) nThreads=atoi(argv[1]);
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

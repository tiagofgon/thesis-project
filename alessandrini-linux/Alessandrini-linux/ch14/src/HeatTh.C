// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File HeaTh.C
//
// Macrotasking version, using SPool and the standard
// Barrier. Compare with the performanc of HeaTh_T.C, which 
// uses the high performance TbbBarrier. 
// ----------------------------------------------------

#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <Rand.h>
#include <CpuTimer.h>
#include <SPool.h>           // new
#include <Barrier.h>       // new
#include <Reduction.h>       // new

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
int  nIter;
bool moreCycles;

Reduction<double>  R;        // new
Barrier   *B;                // new, initialized by main
SPool  *TH;                  // new, initialized by main

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
   if(nIter%stepReport==0) std::cout << "\n Iteration " << nIter << endl;
   if( (error > EPS*initial_error) && (nIter <= maxIts) )
      {
      curr_error = R.Data();
      R.Reset();
      return true;
      }
   else return false;
   }

// ------------------------------
// Task function for SPool pool
// ------------------------------
void TaskFct(void *P)
   {
   int m, n, status, rank;
   int beg, end;
   double error_norm, resid;
   double **swap;

   // --------------------------------
   // First, fix the thread loop range
   // --------------------------------
   beg = 1;
   end = M-1;
   TH->ThreadRange(beg, end);   // [beg, end) becomes thread range
   rank = TH->GetRank();
   do
      {
      error_norm = 0.0;
      for(m=beg; m<end; m++)
            {
            for(n=1; n<(N-1); n++)
               {
               resid = U[m+1][n] + U[m-1][n] + U[m][n-1] + U[m][n+1]
		               - 4 * U[m][n];
	       error_norm += fabs(resid);
               V[m][n] = U[m][n] + 0.25 * resid;    
	       }
	    }
      R.Accumulate(error_norm);
      B->Wait();     // need a barrier here

      if(rank == 1)
         { moreCycles = NextIteration(R.Data()); }
      B->Wait();
      }while(moreCycles);
   }


int main(int argc, char **argv)
   {
   InputData();
   InitJob(M, N);
   CpuTimer TR;
   if(argc==2) nThreads=atoi(argv[1]);

   TH = new SPool(nThreads);    
   B  = new Barrier(nThreads);       // new
   
   TR.Start(); 
   TH->Dispatch(TaskFct, NULL);
   TH->WaitForIdle();
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

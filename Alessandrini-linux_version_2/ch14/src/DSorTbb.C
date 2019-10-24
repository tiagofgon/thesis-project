// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
//
// This is file DSorTbb.C
// 
// We develop a parallel implementation of the SwapDiagonal
// function in DSorb.C, using TBB parallel_for.
//
// This TBB algorithm is ideally suited to this problem,
// because of the dynamic domain decomposition it implements.
// Diagonals have varying sizes, starting from 1, for which
// a parallel treatment is nonsense. Introducing an adequate
// granularity, the parallel_reduce algorithm will increase 
// the number of parallel tasks as the diagonal sizes grow, 
// and reduce it as their sizes decrease. 
// =========================================================

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <CpuTimer.h>
#include <IntRange.h>
#include <Reduction.h>
#include <tbb/blocked_range.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/parallel_reduce.h>

using namespace std;

// ------------------------------------
// Declaration of auxiliary functions
// used by the code
// ------------------------------------
void   InputData();
void   InitJob(int MM, int NN);
void   ExitJob();
void   PrintResult(int step);
void   PrintData();

// Internal data 
// *************
double **U, **F;
double anorm, anormf, delta;
double EPS = 1.0e-5;
double PI = 3.1415926535;
int    nIter;             /* iteration counter */
Reduction<double> Rd;

// Data read from file "sor.dat" 
// *****************************
int N, M, Lgth; 
int maxIts, nTh;
int stepReport;
double Lx, Ly; 
double omega;

// ------------------------------------------------------------
// An instance of this class swaps a descending diagonal of the
// matrix U, starting at site [mi, ni] and descending size
// steps. At each step, mi is descreased and ni is increased
// ------------------------------------------------------------
class SwapDiagonal
   {
   private:
   int mi, ni;
  
   public:
    double diag_error;

   // Constructor
   // -----------
   SwapDiagonal(int _m, int _n) : 
         mi(_m), ni(_n), diag_error(0.0) {}

   SwapDiagonal(SwapDiagonal& SD, tbb::split) :
         mi(SD.mi), ni(SD.ni), diag_error(0.0) {}

   // Task function
   // -------------
   void operator() (const tbb::blocked_range<size_t>& rg) 
      {
      double resid;
      for(size_t k=rg.begin(); k!=rg.end(); ++k)
         {
         resid = U[mi-k+1][ni+k] + U[mi-k-1][ni+k] + U[mi-k][ni+k-1] 
                 + U[mi-k][ni+k+1] - 4 * U[mi-k][ni+k] - F[mi-k][ni+k];
         diag_error += fabs(resid);
         U[mi-k][ni+k] += 0.25 * omega * resid;
         }
      Rd.Accumulate(diag_error);
      }

   void join(const SwapDiagonal& SD)
      {
      diag_error += SD.diag_error;
      }

   };

int main(int argc, char **argv)
   {
   int n, status;
   CpuTimer Trep;
   double swap_error;

   InputData();
   InitJob(M, N);

   //omega = 2.0 / (1.0 + PI/N);
   omega = 1.9;
   int G = N/4;

   tbb::task_scheduler_init init(nTh);
   Trep.Start();
   do
      {
      swap_error = 0.0;
      Rd.Reset();

      // ----------------------------------------------------
      // Swaps diagonals starting from upper left vertex, and
      // including the main diagional
      // ----------------------------------------------------
      for(int m=1; m<M-1; m++) 
          {
          tbb::blocked_range<size_t> Rg(0, m, G);
          SwapDiagonal Sw(m, 1);
          tbb::parallel_reduce(Rg, Sw);
          swap_error += Sw.diag_error;
          }
      // ----------------------------------------------------
      // Swap the remaining diagonals to the right of the
      // main diagonal
      // ----------------------------------------------------
      for(int n=2; n<(N-1); n++) 
          {
          tbb::blocked_range<size_t> Rg(0, N-n-1, G);
          //IntRange Rg(0, N-n-1, G); 
          SwapDiagonal Sw(M-2, n); 
          tbb::parallel_reduce(Rg, Sw);
          swap_error += Sw.diag_error;
          }
      nIter++; 
      if(nIter%stepReport==0) 
         printf("\n Iteradion %d done", nIter);
      }while( (swap_error > EPS*anormf) && (nIter <= maxIts) );
   Trep.Stop();

   printf("\n\n Data: N = %d  M = %d   maxIts = %d\n", N, M, maxIts);
   if(nIter >maxIts) printf("\n Maximal number of iterations exeeded\n");
   else 
      {
      printf("\n Initial error  : %g", anormf);
      printf("\n Final error    : %g", swap_error);
      printf("\n Iterations     : %d", nIter);
      printf("\n Active threads : %d\n", nTh);
      Trep.Report();
      }

   ExitJob();
   return 0;
   }

//////////////////////////////////////////////////////

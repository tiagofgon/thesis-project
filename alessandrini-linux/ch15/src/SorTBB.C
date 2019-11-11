// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
/* This is file SorTBB.C
 * SOR code, with TBB pipelining
 * ======================================
 */

#include "tbb/pipeline.h"
#include "tbb/tick_count.h"
#include "tbb/mutex.h"
#include "tbb/task_scheduler_init.h"
#include <cstdlib>
#include <iostream>
#include <CpuTimer.h>
#include <stdio.h>
#include <math.h>

using namespace std;

// ----------------------------------
// Declaration of auxiliary functions 
// ----------------------------------

void   InputData();
void   InitJob(int MM, int NN);
void   ExitJob();
void   PrintResult(int st);
void   StageRange(int& Beg, int& End, int rank, int nstages);

// ----------------------
// Internal global data 
// ----------------------
double **U, **F;
double anorm, anormf, delta;
double EPS = 1.0e-5;
double PI = 3.1415926535;
int    nIter;             // iteration counter
double omega;

// -------------------------------------
// Global data related to multithreading 
// -------------------------------------
tbb::mutex     MM;

// ------------------------------
// Data read from file "psor.dat" 
// ------------------------------
int N, M, Lgth; 
int maxIts, nTh, nStages;
int stepReport;

//========================
// Pipeline Filter class
//========================
class SORstage: public tbb::filter 
    {
    private:
     int beg, end;       // [beg, end) is x domain allocated to this filter
     int rank;           // rank of this filter
     int m;              // moving row counter, in range [1, M-2]
     void *arg;          // argument passed to next stage

     void  UpdateRow();
     void* operator()(void*);

    public:
     double error_norm;  // accumulates errors computed by this filter

     SORstage();
     void Reset();
     void SetFilter(int r, int ns, int NN);
    };


SORstage::SORstage() : // constructor
    filter(true), m(1), error_norm(0.0)
    { }

 
void SORstage::Reset()
    {
    m = 1;            // reset current row index
    error_norm = 0.0;
    }


// Private auxiliary function
// --------------------------
void SORstage::UpdateRow()
   {
   for(int n=beg; n<end; n++)
      {
      double resid = U[m+1][n] + U[m-1][n] + U[m][n-1] + U[m][n+1]
                     - 4 * U[m][n] - F[m][n];
      error_norm += fabs(resid);
      U[m][n] += 0.25 * omega * resid;
      }
   }


void SORstage::SetFilter(int r, int nst, int NN)
    {
    // nst is the number of stages in the pipeline
    // ------------------------------------------
    rank = r;                            // ranges start from 1
    beg = 1;
    end = NN-1;
    StageRange(beg, end, r, nst);       // initializes beg, end
    arg = static_cast<void *> (&nIter);  // irrelevant, to have a valid address
    std::cout << "\n Filter " << rank << " range: " << beg << ", " << end
                              << std::endl;
    }


void* SORstage::operator()(void* p) 
    {
    if(rank==1)     // code for first filter 
        {
        if(m==(M-1)) return NULL;
        else
           {
           UpdateRow();
           m++;
           return arg;
           }
        }

    if(rank>1)      // code for next filters       
        {
        if(p==NULL) return NULL;
        else
            {
            UpdateRow();
            m++;
            return arg;
            }
        }
    }

// ==============================
// MAIN CODE
// ==============================
int main(int argc, char **argv)
   {
   int n, status;
   CpuTimer TR;

   InputData();
   InitJob(M, N);
   omega = 2.0 / (1.0 + PI/N);
   //omega = 1.9;
   if(argc==2) nTh = atoi(argv[1]);

   // --------------------
   // start task scheduler
   // --------------------
   tbb::task_scheduler_init init(nTh);

   // -----------------------------
   // construct the pipeline stages
   // -----------------------------
   tbb::pipeline pipeline;
   SORstage ST[16];
   for(n=1; n<=nTh; n++) ST[n].SetFilter(n, nTh, N);
   for(n=1; n<=nTh; n++) pipeline.add_filter( ST[n] );

   TR.Start();
   do
      {
      for(n=1; n<=nTh; n++) ST[n].Reset();
      pipeline.run(nTh);
      nIter++;
      anorm = 0.0;
      for(n=1; n<=nTh; ++n) anorm += ST[n].error_norm;
      if(nIter%stepReport==0) 
          printf("\n Iteration %d performed", nIter); 
      }while( (anorm > EPS*anormf) && (nIter <= maxIts) );
   TR.Stop();
   
   printf("\n\n Data: N = %d  M = %d   maxIts = %d\n", N, M, maxIts);
   if(nIter >maxIts) printf("\n Maximal number of iterations exeeded\n");
   else 
      {
      printf("\n Initial error  : %g", anormf);
      printf("\n Final error    : %g", anorm);
      printf("\n Iterations     : %d", nIter);
      printf("\n Active threads : %d\n", nTh);
      TR.Report();
      }
   PrintResult(20);
   ExitJob();
   }

// ------------------------------------------------
// Auxiliary function, to get the stage index range 
// from the global stage rank (the order in which
// it has been inserted in the pipeline).
// NOTICE : global rank should be in [1, nThreads].
// ON OUTPUT, index range is [beg, end).
// ------------------------------------------------
void StageRange(int& Beg, int& End, int rank, int nstages)
   {
   int n, b, e;
   int size, D, R;
   
   size = End-Beg;
   D = (size/nstages);
   R = size%nstages;

   e = Beg;
   for(n=1; n<=rank; n++)
      {
      b = e;
      e = b+D;
      if(R)
         {
         e++;
         R--;
         }
      }
   Beg = b;
   End = e;
   }


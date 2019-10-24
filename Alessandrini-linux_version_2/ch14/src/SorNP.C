// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
//
// This is file SorNP.C
//
// Macrotasking computation, using the NPool thread
// pool..
//
// This version of the code uses the standard Barrier
// utility
// **************************************************
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <Barrier.h>
#include <NPool.h>
#include <Reduction.h>
#include <CpuTimer.h>

// ------------------------------------
// Declaration of auxiliary functions
// used by the code
// ------------------------------------
void   InputData();
void   InitJob(int MM, int NN);
void   ExitJob();
void   PrintResult(int n);
void   PrintData();

// ---------------------
// Internal global data 
// ---------------------
double **U, **F;
double anorm, anormf, delta, omega;
const double EPS = 1.0e-5;
const double PI  = 3.1415926535;
int    nIter;             // iteration counter 

// -------------------------------------
// Global data related to multithreading 
// --------------------------------------
Barrier          *B;         // barrier
NPool            *TP;       // thread pool
Reduction<double> RD;       // reduction of global error

// -----------------------------
// Data read from file "sor.dat"
// -----------------------------
int N, M, Lgth;
int maxIts, nTh, stepReport;


// Auxiliary function: determines the loop thread
// range, given the thread rank in [1, Nthreads]
// ----------------------------------------------         
void ThreadRange(int& Beg, int& End, int rank)
   {
   int n, beg, end;
   int size, D, R;

   size = End-Beg;
   D = (size/nTh);
   R = size%nTh;

   end = Beg;
   for(n=1; n<=rank; n++)
      {
      beg = end;
      end = beg+D;
      if(R)
         {
         end++;
         R--;
         }
      }
   Beg = beg;
   End = end;
   std::cout << "\n Thread " << rank << " range: " << beg << "   " << end
             << std::endl;
   }

// -----------------------------
// The task used in this problem.
// -----------------------------
class SorTask : public Task
   {
   private:
    int rank;      // The rank of this thread
    int beg, end;  // The loop range of this thread

   public:
    SorTask(int R) : rank(R), beg(1), end(M-1) 
       { ThreadRange(beg, end, rank); }

    void ExecuteTask()
       {
       int n, m;
       int mSh, nSh, pass;
       double resid, error_norm;

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
		 
             if(pass==2) RD.Accumulate(error_norm);
             B->Wait();  
             }   // end of pass loop

         // Enter sequential region
         // -----------------------
         if(rank==1)
	     {
	     anorm = RD.Data();
	     RD.Reset();
	     nIter++;
             if(nIter%2000==0) printf("\n Iteration %d done", nIter);
	     } 
         // --------------------------------------------------------
         // Exit sequential region 
         B->Wait();
         }while( (anorm > EPS*anormf) && (nIter <= maxIts) );
       }
    };


int main(int argc, char **argv)
   {
   int n, status;

   omega = 1.8;
   InputData();
   InitJob(M, N);
   CpuTimer TR;

   if(argc==2) nTh=atoi(argv[1]);
   TP = new NPool(nTh);
   B = new Barrier(nTh);

   // Construct parallel job
   // ----------------------
   TaskGroup *TG = new TaskGroup();
   for(int n=1; n<=nTh; ++n)
      {
      SorTask *t = new SorTask(n);
      TG->Attach(t);
      }

   // Submit and wait for job
   // -----------------------
   TR.Start(); 
   int jobid = TP->SubmitJob(TG);
   TP->WaitForJob(jobid);
   TR.Stop();

   PrintData();
   PrintResult(50);
   TR.Report();
   delete B;
   delete TP;
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


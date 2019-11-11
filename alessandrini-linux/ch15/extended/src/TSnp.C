// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
//
// This is file TSnp.C
//
// Macrotasking computation, using the NPool thread
// pool..
//
// We are using here the TBB enumerable_thread_specific
// class to perform reductions
//
// Pipelining tasks. A task to update a sequence of sites
// in a row, is launched as soon as parent tasks are finished.
// An integer matrix ID counts the number of unresolved task
// dependencies.
// *********************************************************

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <NPool.h>
#include <CpuTimer.h>
#include <atomic>
#include <tbb/enumerable_thread_specific.h>

typedef tbb::enumerable_thread_specific<double> StorageType;

struct Node
   {
   std::atomic<int> ref_count;
   };

double finit()
   { return 0.0; }

// Functor to feed the "combine" algorithm
// ---------------------------------------
class AddTls
   {
   public:
    double operator()(double a, double b)
       {
       return (a+b);
       }
   };

// ------------------------------------
// Declaration of auxiliary functions
// used by the code
// ------------------------------------
void   InputData();
void   InitJob(int MM, int NN);
void   ExitJob();
void   PrintResult(int n);
void   PrintData();

Node **AllocNodeMatrix(int M, int N);
void FreeNodeMatrix(Node **d);
void InitializeNodes();

// ---------------------
// Internal global data 
// ---------------------
double **U, **F;
Node   **PR;

StorageType Gerror(finit);   // for global errors 

double anorm, anormf, delta, omega;
const double EPS = 1.0e-5;
const double PI  = 3.1415926535;
int    nIter;             // iteration counter 

// -------------------------------------
// Global data related to multithreading 
// --------------------------------------
NPool            *TP;            // thread pool
std::atomic<int> task_count;     // to track possible race condition

// -----------------------------
// Data read from file "sor.dat"
// -----------------------------
int N, M, Lgth;
int maxIts, nTh, stepReport;

// -----------------------------
// The task used in this problem.
// -----------------------------
class SorTask : public Task
   {
   private:
    int m, s;  // target site
    int step;
    int beg, end;

   public:
    SorTask(int mm, int ss) : m(mm), s(ss) 
       { 
       step = (N-2)/nTh;
       beg = 1 + s*step;
       end = 1 + (s+1)*step;
       if(s==(nTh-1)) end = N-1;
       }

    void ExecuteTask()
       {
       double resid, error;
       StorageType::reference my_error = Gerror.local();

       error = 0.0;
       for(int n=beg; n!=end; n++)
          {
          resid = U[m+1][n] + U[m-1][n] + U[m][n-1] + U[m][n+1]
                      - 4 * U[m][n] - F[m][n];
          error += fabs(resid);
          U[m][n] += 0.25 * omega * resid;
          }    
       my_error += error;
       task_count++;

       // ----------------------------------------------------
       // Decrease the dependence count of dependent sites, and
       // launch tasks if appropriate.
       // ----------------------------------------------------
       if(s < (nTh-1))
          {
          if (--(PR[m][s+1].ref_count) == 0)
              {
              SorTask *t = new SorTask(m, s+1);
              TP->SpawnTask(t, false);
              }
          }
       if(m > 1)
          {
          if (--(PR[m-1][s].ref_count) == 0)
              {
              SorTask *t = new SorTask(m-1, s);
              TP->SpawnTask(t, false);
              }
          }
       }
   };  


int main(int argc, char **argv)
   {
   int n, status;
   AddTls my_functor;

   InputData();
   InitJob(M, N);
   PR = AllocNodeMatrix(M, N);     // new
   CpuTimer TR;

   if(argc==2) nTh=atoi(argv[1]);
   TP = new NPool(nTh);

   // ------------------------------------------------------
   // Submit a single task job, the one that starts updating
   // the upper left corner
   // ------------------------------------------------------
   TR.Start(); 
   InitializeNodes();
   nIter = 0;
   //omega = 1.9;
   omega = 2.0/(1.0+PI/N);
   task_count = 0;
   do
      {
      SorTask *t = new SorTask(M-2, 0);
      int jobid = TP->SubmitJob(t);
      TP->WaitForJob(jobid);
      
      anorm = Gerror.combine(my_functor);  // error reduction
      nIter++;
      
      if(task_count != (N-2)*nTh)
          std::cout << "\n Warning - task_count = " << task_count
                    << std::endl;

      if(nIter%stepReport==0) 
         {
         printf("\n Iteration %d done", nIter);
         printf("\n Error : %f", anorm);
         }

      // We must now reset the TSL values
      // --------------------------------
      for(StorageType::iterator I = Gerror.begin(); I != Gerror.end(); I++)
      *I = 0.0;
      InitializeNodes();
      task_count = 0;
      }while( (anorm > EPS*anormf) && (nIter <= maxIts) );
   
   TR.Stop();
   PrintData();
   PrintResult(10);
   TR.Report();
   delete TP;
   FreeNodeMatrix(PR);
   ExitJob();
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

// ---------------------------- 
// Memory management functions 
// ----------------------------
Node **AllocNodeMatrix(int M, int N)
    {
    int n;
    Node **d;
	
    d = (Node **)malloc( M*sizeof(Node *) );
    if(!d)
	{	
	printf("\n malloc failed for N\n");
	exit(0);
	}
    d[0] = (Node *)malloc ( (M*N)*sizeof(Node) );
    if(!d[0]) 
	{
	printf("\n malloc failed for N[O]\n");
	exit(0);
	}
    for(n=1; n<M; n++) d[n] = d[n-1] + N;
    return d;
    }

void FreeNodeMatrix(Node **d)
    {
    free(d[0]);
    free(d);
    }

// This function sets the number of dependencies at each site
// ----------------------------------------------------------
void InitializeNodes()
   {
   int m, s;
   for(s=0; s<nTh; s++) PR[M-2][s].ref_count = 1;
   for(m=M-3; m>0; m--)
      {
      PR[m][0].ref_count = 1;
      for(s=1; s<nTh; s++) PR[m][s].ref_count = 2;
      }
   }


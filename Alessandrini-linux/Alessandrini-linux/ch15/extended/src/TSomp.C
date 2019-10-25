// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
//
// This is file TSomp.C
//
// Macrotasking computation, using the OpenMP taskgroup
// facility
//
// Pipelining tasks. A task to update a sequence of sites
// in a row, is launched as soon as parent tasks are finished.
// An integer matrix ID counts the number of unresolved task
// dependencies.
// **************************************************
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <CpuTimer.h>
#include <omp.h>
#include <atomic>

struct Node
   {
   std::atomic<int> ref_count;
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

Node   **AllocNodeMatrix(int M, int N);
void   FreeNodeMatrix(Node **d);
void   InitializeNodes();
double StoreTls(double d, bool b);

// ---------------------
// Internal global data 
// ---------------------
double **U, **F;
Node   **PR;

double anorm, anormf, delta, omega;
const double EPS = 1.0e-5;
const double PI  = 3.14159265;
int    nIter;             // iteration counter 

// -------------------------------------
// Global data related to multithreading 
// --------------------------------------
std::atomic<int> task_count;     // to track possible race conditions

// -----------------------------
// Data read from file "sor.dat"
// -----------------------------
int N, M, Lgth;
int nTh;                     // number of threads
int maxIts, stepReport;

// -------------------------------------------------
// This is the function used in this problem as a
// basic OpenMP task. The constructor inputs are
// the matriw row m, and the row sector s, where
// s = 0, 1, ..., nTh-1
// -------------------------------------------------
void SorTask(int m, int s) 
   {
   double resid;
   double error;

   // ---------------------------------------------------------
   // First, compute the limits of the sector, using the global
   // variable nTh (number of threads) and N (row size)
   // ---------------------------------------------------------
   int step = (N-2)/nTh;
   int beg = 1 + s*step;
   int end = 1 + (s+1)*step;
   if(s==(nTh-1)) end = N-1;

   // --------------------------
   // Now, update the row sector
   // --------------------------
   error = 0.0;
   for(int n=beg; n<end; n++)
      {
      resid = U[m+1][n] + U[m-1][n] + U[m][n-1] + U[m][n+1]
                      - 4 * U[m][n] - F[m][n];
      error += fabs(resid);
      U[m][n] += 0.25 * omega * resid;  
      }  
   StoreTls(error, true);             // thread local storage
   task_count++;

   // ----------------------------------------------------
   // Decrease the dependence count of dependent sites, and
   // launch tasks if appropriate.
   // ----------------------------------------------------
   if(s < (nTh-1))
      {
      if (--(PR[m][s+1].ref_count) == 0)
         #pragma omp task
         { SorTask(m, s+1); }
      }
   if(m > 1)
      {
      if (--(PR[m-1][s].ref_count) == 0)
         #pragma omp task
         { SorTask(m-1, s); }
      }
   }  


int main(int argc, char **argv)
   {
   int n, status;
   InputData();
   InitJob(M, N);
   PR = AllocNodeMatrix(M, nTh);     // new
   CpuTimer TR;

   if(argc==2) nTh=atoi(argv[1]);

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

   omp_set_num_threads(nTh);
   do
      {
      anorm = 0.0;     // initialize global error

      // A parallel job to update the U matrix
      // -------------------------------------
      #pragma omp parallel reduction(+:anorm)
         {
         #pragma omp single 
            {
            #pragma omp taskgroup
               {
               #pragma omp task
                  { SorTask(M-2, 0); }
               }
            }
         
         // back to parallel region. Implicit barrier here
         // ----------------------------------------------
         anorm = StoreTls(0.0, false);
         }

      nIter++;      
      if(task_count != (M-2)*nTh)
          std::cout << "\n Warning - task_count = " << task_count
                    << std::endl;

      if(nIter%stepReport==0) 
         {
         printf("\n Iteration %d done", nIter);
         printf("\n Error : %f", anorm);
         }

      InitializeNodes();
      task_count = 0;
      }while( (anorm > EPS*anormf) && (nIter <= maxIts) );
   
   TR.Stop();
   PrintData();
   PrintResult(10);
   TR.Report();
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

double StoreTls(double d, bool b)
   {
   static double partial_error = 0.0;
   #pragma omp threadprivate(partial_error)  // notice the simplicity

   double retval = 0.0;
   if(b==true) partial_error += d;           // standard partial reduction
   else 
       {                                     
       retval = partial_error;               // return stored threadprivate
       partial_error = 0.0;                  // initialize for next iteration
       }
    return retval;
   }


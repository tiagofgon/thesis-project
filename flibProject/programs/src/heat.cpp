/* ---------------------------------------------------------------------------
                           Made by Tiago Gonçalves - 2019
 --------------------------------------------------------------------------*/

// File HeatNP.cpp
// -----------
// Macrotasking code, using the NPool pool
// ---------------------------------------
#include <CpuTimer.hpp>
#include <NPool.hpp>        
#include <math.h>        
#include <TBarrier.hpp> 
#include <Reduction.hpp>  

// Global variables
// -----------------
NPool             *TP;
TBarrier        *B;
Reduction<double> R;
bool              moreCycles;

double **U, **V;
const double EPS = 1.0e-5;
double initial_error, curr_error;
int nIter;

// Data read from file "heat.dat"
// ------------------------------
int N, M;
int maxIts;
int nThreads;
int stepReport;

// Declaration of auxiliary functions
// ----------------------------------
void InputData();
void InitJob(int MM, int NN);
void ExitJob();
void PrintResult(int n);

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
       R.Reset();
       return true;
       }
   else return false;
   }

// Auxiliary function: determines the loop thread
// range, given the thread rank in [1, Nthreads]
// ----------------------------------------------         
void ThreadRange(int& Beg, int& End, int rank)
   {
   int n, beg, end;
   int size, D, R;

   size = End-Beg;
   D = (size/nThreads);
   R = size%nThreads;

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


int main(int argc, char **argv)
   {
   CpuTimer TR;
   InputData();
   InitJob(M, N);
   if(argc==2) nThreads = atoi(argv[1]);

   TP = new NPool(nThreads);
   B  = new TBarrier(nThreads);


   std::vector<std::future<int>> futures;

   auto heatTask = [](int rankk) {
      int rank=rankk;      // The rank of this thread
      int beg=1, end=M-1;  // The loop range of this thread
      int apagar=0;
      ThreadRange(beg, end, rank);
      double error, thread_error;
      int m, n;
      moreCycles = true;
      do {
         apagar++;
         thread_error = 0.0;
         for(m=beg; m<end; m++) {
            for(n=1; n<(N-1); n++) {
               error = U[m+1][n] + U[m-1][n] + U[m][n-1] + U[m][n+1]
	 	            - 4 * U[m][n];
	            thread_error += fabs(error);
               V[m][n] = U[m][n] + 0.25 * error;    
	         }
	      }

         R.Accumulate(thread_error);
         B->Wait();
         if(rank==1) {
            moreCycles = NextIteration(R.Data()); 
            // cout << moreCycles << endl;
         }
         B->Wait();
         }while(moreCycles);
         // cout << "ola-------" << endl;
      return 0;
      //cout << "cicles " << apagar << endl;
   };




   // Construct parallel job
   // ----------------------
   TaskGroup *TG = new TaskGroup();
   for(int n=1; n<=nThreads; ++n) {
      Task *t = new Task();
      auto future = t->insertTask(heatTask, n);
      futures.emplace_back(std::move(future));
      TG->Attach(t);
      

      }

   // Submit and wait for job
   // -----------------------
   TR.Start(); 
   int jobid = TP->SubmitJob(TG);
   TP->WaitForJob(jobid);
   TR.Stop();

   for (auto& future : futures) {
        auto a = future.get();
   }

   PrintResult(50);
   TR.Report();
   delete B;
   delete TP;
   }

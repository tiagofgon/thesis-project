// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File McpiRec.C
// 
// Recursive MonteCarlo computation of PI,  given 
// nSamples and granularity criterion to stop
// recurrence
// ------------------------------------------------

#include <iostream>
#include <sstream>
#include <math.h>
#include <NPool.h>
#include <Reduction.h>
#include <Rand.h>
#include <SafeCounter.h>
#include <SafeCout.h>

// Global data
// -----------
NPool          *NP;         // thread pool
Reduction<long> RA;         // to perform acceptance reduction
SafeCounter     SC;         // to initialize task private random gens
SafeCout        Scout;      // used to order output to sodout
long            G;          // granularity
long            nSamples;   // number of random samples


// -------------------------------------------------------------
// Generic, but recursive task class
//
// Constructor receives the nSamples, the number of MC events 
// that the routine is asked to execute.
//
// In this problem, the granularity G is the maximum number of
// samples that the task is allowed to handle directly. If the 
// requested nSamples is bigger than G, the task spawns two childs
// for alf of the samples, and terminates.
// Otherwise, it performs the MC run and accumulates the acceptances
// in a global Reduction object.
// ------------------------------------------------------------------

class McpiTask: public Task
   {
   private:
    long nS;
    ostringstream os;

   public:
    McpiTask(long l): Task(), nS(l){}
    void ExecuteTask()
       {
       long count = 0;
       double x, y;

       if(nS>G)
          {
          long nS1 = nS/2;            // divide nSamples by 2
          McpiTask *T1 = new McpiTask(nS1); 
          McpiTask *T2 = new McpiTask(nS1+nS%2);   // include the rest 
          NP->SpawnTask(T1, false);
          NP->SpawnTask(T2, false);
          // ------------------------------------------------------
          os << "\n Task with " << nS << " samples submitted two childs" ;
          Scout.Flush(os);
          // ------------------------------------------------------
          }
      else
         {
         // Here, nS < G. Perform the computation
         // -------------------------------------
         int rank = SC.Next();
         Rand R(999*rank);

         for(size_t n=0; n<nS; ++n)
            {
            x = R.draw();
            y = R.draw();
            if( (x*x+y*y) <= 1.0 ) count++;
            }
         RA.Accumulate(count);
         // ------------------------------------------
         os << "\n Task with " << nS << " terminated";
         Scout.Flush(os);
         // ------------------------------------------
         }
      }
   //end of class  
   };


// The main function
// *****************
int main (int argc, char *argv[])
   {
   int Nth, jobID;
   long accepted, total;
   double pi;

   // Get data from command line
   // --------------------------
   if(argc == 2) Nth = atoi(argv[1]);
   else Nth = 2; 
   G = 100000000;
   nSamples = 400000000;

   // Initialize the thread pool
   // --------------------------
   NP = new NPool(Nth, 20);

   // Submit single task job, and wait for idle
   // -----------------------------------------
   std::cout << "\n Initial number of samples is " << nSamples
             << std::endl;
   McpiTask *T = new McpiTask(nSamples);
   jobID = NP->SubmitJob(T);
   NP->WaitForJob(jobID);

   accepted = RA.Data(); 
   pi = 4.0 * (double)accepted / nSamples;

   std::cout << "\n PI = " <<  pi << std::endl;
   return 0;
   }

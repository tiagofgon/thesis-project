// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// TBkBarrier.C
// Testing the BkBarrier barrier, with OpenMP threads
//
// Threads are successively blocked and then released
// by main().
// --------------------------------------------------

#include <iostream>
#include <BkBarrier.h>
#include <Timer.h>
#include <RandInt.h>
#include <omp.h>

BkBarrier *BB;

// ------------------------------------------------------------
// Note on RandInt objects:
// -----------------------
// RandInt(N) RI;     // declare object
// RI.draw()  returns random integer in [0, N].
//
// In our case (see below) r.draw() returns a random integer in 
// [0, 1000]. This is used by the Timer to implement a random wait 
// of less than 1 second
// -------------------------------------------------------------
RandInt r(1000);

// ---------------------------------------------------------
// This task function executes 4 loop iterations, in which
// it is blocked in a BkBarrier at each iteration. Main()
// will be in charge of releasing them. This code establishes
// a synchronization between the master thread in the worker
// team, and the remaining worker threads.
// ----------------------------------------------------------
void task()
   {
   int rank = omp_get_thread_num();
   Timer T;
   if(rank==0)  
      {
      // master thread driving the computation
      // ----------------------------------------------------
      T.Wait(500);
      for(int n=0; n<4; ++n)
         {
         BB->WaitForIdle();
         std::cout << "\n ------------------------------" << std::endl;
         BB->ReleaseThreads();
         }
      // -----------------------------------------------------
      }
   else         // remaining worker threads reacting
      {
      // worker threads, reacting to master thread
      // ------------------------------------------------------
      for(int k=0; k<4; ++k)
         {
         T.Wait(500+r.draw());
         std::cout << "\n Rank " << rank << " thread reporting" 
                   << std::endl;
         BB->Wait();
         }
      // ------------------------------------------------------
      }
   }


int main(int argc, char *argv[])
   {
   int nTh;

   if(argc == 2) nTh = atoi(argv[1]); 
   else nTh = 2;

   BB = new BkBarrier(nTh);
   omp_set_num_threads(nTh+1);  // notice! master thread is in the team
   #pragma omp parallel
      {
      task();
      }
   delete BB;
   return 0;
   }




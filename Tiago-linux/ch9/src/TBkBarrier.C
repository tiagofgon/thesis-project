// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// TBkBarrier.C
// Testing the BkBarrier barrier
//
// Threads are successively blocked and then released
// by main().
// --------------------------------------------------

#include <iostream>
#include <BkBarrier.h>
#include <Timer.h>
#include <RandInt.h>
#include <SPool.h>

BkBarrier *BB;
SPool     *TS;

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
// a synchronization between the worker threads and the main
// thread (not in the worker team)
// ----------------------------------------------------------
void task(void *p)
   {
   int rank = TS->GetRank();
   Timer T;
   for(int k=0; k<4; ++k)
      {
      T.Wait(500+r.draw());
      std::cout << "\n Rank " << rank << " thread reporting" << std::endl;
      BB->Wait();
      }
   std::cout << "\n Rank " << rank << " thread exiting" <<  std::endl;
   }


int main(int argc, char *argv[])
   {
   int n, nTh;
   Timer T;
   if(argc == 2) nTh = atoi(argv[1]); 
   else nTh = 2;

   BB = new BkBarrier(nTh);
   TS = new SPool(nTh);
   TS->Dispatch(task, NULL);         // launch worker threads

   T.Wait(500);
   for(n=0; n<4; ++n)   // release the workers 4 times
      {
      BB->WaitForIdle();
      std::cout << "\n ------------------------------" << std::endl;
      BB->ReleaseThreads();
      }

   TS->WaitForIdle();
   delete BB;
   delete TS;
   return 0;
   }




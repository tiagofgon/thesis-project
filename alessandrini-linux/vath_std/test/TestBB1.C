// **************************************
// Copyright (c) 2011 Victor Alessandrini
// All rights reserved.
// **************************************
// TestBB1.C
// First test of BkBarrier class
// ----------------------------------------

#include <iostream>
#include <BkBarrier.h>
#include <Timer.h>
#include <RandInt.h>
#include <thread>

std::thread **WT;
BkBarrier *BB;
RandInt r(1000);

void task(int rank)
   {
   Timer T;
   for(int k=0; k<4; k++)
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
   std::cout << "\n *** First test of BkBarrier class\n" << std::endl;

   // Launch nTh identical threads
   // ----------------------------
   WT = new std::thread*[nTh+1];
   for(int n=1; n<=nTh; ++n)
      WT[n] = new std::thread(task, n);
 
   T.Wait(500);
   for(n=0; n<4; n++)
      {
      BB->WaitForIdle();
      std::cout << "\n ------------------------------" << std::endl;
      BB->ReleaseThreads();
      }

   for(int n=1; n<=nTh; n++) WT[n]->join();

   delete BB;
   for(n=1; n<=nTh; ++n) delete WT[n];
   delete [] WT;
   return 0;
   }




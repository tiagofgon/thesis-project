// **************************************
// Copyright (c) 2011 Victor Alessandrini
// All rights reserved.
// **************************************
// TestRank.C
//
// Testing the GetRank() function in ThPeer
// ----------------------------------------

#include <iostream>
#include <stdlib.h>
#include <SPool.h>
#include <Timer.h>
#include <RandInt.h>

SPool *TP;
RandInt r(1000);

void task(void *p)
   {
   Timer T;
   T.Wait(500+r.draw());
   int rank = TP->GetRank();
   std::cout << "\n Rank " << rank << " thread exiting" << std::endl;
   }

int main(int argc, char *argv[])
   {
   int nTh;
   if(argc == 2) nTh = atoi(argv[1]); 
   else nTh = 2;

   std::cout << "\n *** Testing GetRank() in SPool class\n " 
              << std::endl;

   TP = new SPool(nTh);
   TP->Dispatch(task, NULL);
   TP->WaitForIdle();

   delete TP;
   return 0;
   }




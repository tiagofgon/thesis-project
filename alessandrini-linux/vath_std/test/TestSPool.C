// **************************************
// Copyright (c) 2011 Victor Alessandrini
// All rights reserved.
// **************************************
// TestSPool.C
// ----------------------------------------

#include <iostream>
#include <SPool.h>
#include <Timer.h>
#include <RandInt.h>

SPool TP(2);
RandInt r(1000);

void task(void *p)
   {
   int rank;
   Timer T;
   rank = TP.GetRank();
   T.Wait(500+r.draw());
   std::cout << "\n Rank " << rank << " thread reporting" << std::endl;
   }


int main(int argc, char *argv[])
   {
   int n, nTh;
   Timer T;
   //if(argc == 2) nTh = atoi(argv[1]); 
   //else nTh = 2;

   //TP = new SPool(nTh);
   std::cout << "\n *** Testing SPool thread pool\n " << std::endl;

   for(n=0; n<3; n++)
      {
      TP.Dispatch(task, NULL);
      TP.WaitForIdle();
      std::cout << "\n ------------------------------" << std::endl;
      }
   //delete TP;  
   return 0;
   }




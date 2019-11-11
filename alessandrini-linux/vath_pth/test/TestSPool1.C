// **************************************
// Copyright (c) 2011 Victor Alessandrini
// All rights reserved.
// **************************************
// TestSPool1.C - Same as TestPool.C, but
// now we have a global pool instead of a
// pointer to the pool.
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
   std::cout << "\n SPool test 2 " << std::endl;

   for(n=0; n<3; n++)
      {
      TP.Dispatch(task, NULL);
      TP.WaitForIdle();
      std::cout << "\n ------------------------------" << std::endl;
      }  
   return 0;
   }




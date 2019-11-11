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
#include <pthread.h>

pthread_t threads[16];
int rank[16];
BkBarrier *BB;
RandInt r(1000);

void *task(void *p)
   {
   int rank;
   Timer T;
   rank = *(int *)p;
   for(int k=0; k<4; k++)
      {
      T.Wait(500+r.draw());
      std::cout << "\n Rank " << rank << " thread reporting" << std::endl;
      BB->Wait();
      }
   std::cout << "\n Rank " << rank << " thread exiting" <<  std::endl;
   return NULL;
   }


int main(int argc, char *argv[])
   {
   int n, nTh;
   Timer T;
   if(argc == 2) nTh = atoi(argv[1]); 
   else nTh = 2;
   std::cout << "\n First test of BkBarrier class " << std::endl;

   BB = new BkBarrier(nTh);

   // Launch nTh identical threads
   // ----------------------------
   for(n=0; n<16; n++) rank[n] = n; 
   for(n=0; n<nTh; n++)
      pthread_create(&threads[n], NULL, task, (void *)&rank[n]);
   T.Wait(500);
   for(n=0; n<4; n++)
      {
      BB->WaitForIdle();
      std::cout << "\n ------------------------------" << std::endl;
      BB->ReleaseThreads();
      }

   for(int n=0; n<nTh; n++)
      pthread_join(threads[n], NULL);

   delete BB;
   return 0;
   }




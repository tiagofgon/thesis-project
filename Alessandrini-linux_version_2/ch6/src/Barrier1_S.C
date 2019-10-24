// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// *****************************************
// File Barrier1_S.C
//
// This example shows how to construct a Barrier synchronization
// tool using the C++11 idle wait protocol. The implementation
// is very simple, but this barrier cannot be reused in the same
// code.
// ------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <mutex>
#include <condition_variable>
#include <SPool.h>

int     count, Nth;
SPool   *TS;
std::mutex              count_mutex; 
std::condition_variable count_cond;

// This is a very simple function called by worker threads
// to implement a barrier synchronization point.
// -------------------------------------------------------
void BarrierWait(int R)
   {
   // -----------------------------------------------------
   // Initially, count=Nth (number of cooperating threads)
   // Caller thread acquire mutex and decreases count. 
   // - If count>0, wait on condition. 
   // - If count==0, print ID and broadcast wake up.
   // ----------------------------------------------------
   std::unique_lock<std::mutex> lock(count_mutex);
   count--;
   if(count)
      {
      while(count) count_cond.wait(lock);
      }
   else
      {
      printf("\n Broadcast sent by thread %d\n\n", R);
      count_cond.notify_all();
      }
   }

// -------------------
// Worker threads code
// -------------------

void worker_thread(void *idp)
   {
   int my_rank = TS->GetRank();
   printf("Thread %d before barrier\n", my_rank);
   BarrierWait(my_rank);
   printf("Thread %d after barrier\n", my_rank);
   }   

// ------------------------
// Main, always the same...
// ------------------------

int main(int argc, char **argv)
   {
   if(argc==2) Nth = atoi(argv[1]);
   else Nth = 2;
   count = Nth;

   TS = new SPool(Nth);
   TS->Dispatch(worker_thread, NULL);
   TS->WaitForIdle();
   return 0;
   }

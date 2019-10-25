// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// *****************************************
// File Barrier2_S
//
// Modifies the Barrier1_S.C example, by constructing
// now a reusable Barrier synchronization utility
// --------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <mutex>
#include <SPool.h>

int     count, Nth;
bool    predicate;
SPool   *TS;
std::mutex              count_mutex;
std::condition_variable count_cond; 

// Auxiliary function called by worker threads
// -------------------------------------------
void BarrierWait(int R)
   {
   // -----------------------------------------------------
   // Acquire mutex and decrease count. 
   // - If count>0, wait for the predicate to be is toggled. 
   // - If count==0, toggle the predicate, reinitialize count,
   //   print ID and broadcast wake up.
   // ------------------------------------------------------
   std::unique_lock<std::mutex> lock(count_mutex);
   bool my_flag = predicate;
   count--;
   if(count)
      {
      while(my_flag == predicate) 
          count_cond.wait(lock);
      }
   else
      {
      predicate = !predicate;
      count = Nth;
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
   printf("Thread %d before first barrier\n", my_rank);
   BarrierWait(my_rank);
   printf("Thread %d before second barrier\n", my_rank);
   BarrierWait(my_rank);
   printf("Thread %d after all barriers\n", my_rank);
   }   

// ------------------------
// Main, always the same...
// ------------------------

int main(int argc, char **argv)
   {
   if(argc==2) Nth = atoi(argv[1]);
   else Nth = 2;
   count = Nth;
   predicate = true;

   TS = new SPool(Nth);
   TS->Dispatch(worker_thread, NULL);
   TS->WaitForIdle();
   return 0;
   }

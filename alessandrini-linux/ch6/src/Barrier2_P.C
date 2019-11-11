// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File Barrier2_P.C
// 
// Modifies the Barrier1_P.C example, by constructing
// now a reusable Barrier synchronization utility
// --------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <SPool.h>

int     count, Nth;
bool    predicate;
SPool   *TS;
pthread_mutex_t count_lock = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t count_cond  = PTHREAD_COND_INITIALIZER;

// Auxiliary function called by worker threads
// -------------------------------------------
void BarrierWait(int R)
   {
   bool my_flag = predicate;
   // -----------------------------------------------------
   // Acquire mutex and decrease count. 
   // - If count>0, wait for the predicate to be is toggled. 
   // - If count==0, toggle the predicate, reinitialize count,
   //   print ID and broadcast wake up.
   // ------------------------------------------------------
   pthread_mutex_lock(&count_lock);
   count--;
   if(count)
      {
      while(my_flag == predicate) 
          pthread_cond_wait(&count_cond, &count_lock);
      pthread_mutex_unlock(&count_lock);
      }
   else
      {
      predicate = !predicate;
      count = Nth;
      printf("\n Broadcast sent by thread %d\n\n", R);
      pthread_cond_broadcast(&count_cond);
      pthread_mutex_unlock(&count_lock);
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

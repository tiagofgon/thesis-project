// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// *******************************************
// File Barrier1_P.C
//
// This example shows how to construct a Barrier synchronization
// tool using the Pthreads idle wait protocol. The implementation
// is very simple, but this barrier cannot be reused in the same
// code.
// -------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <SPool.h>

int     count, Nth;
SPool   *TS;
pthread_mutex_t count_lock = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t count_cond  = PTHREAD_COND_INITIALIZER;

// -------------------------------------------------------
// This is a very simple function called by worker threads
// to wait for the arrival of all the cooperating threads 
// ------------------------------------------------------
void BarrierWait(int R)
   {
   // -----------------------------------------------------
   // Initially, count=Nth (number of cooperating threads)
   // Caller thread acquire mutex and decreases count. 
   // - If count>0, wait on condition. 
   // - If count==0, print ID and broadcast wake up.
   // ----------------------------------------------------
   pthread_mutex_lock(&count_lock);
   count--;
   if(count)
      {
      while(count) 
          pthread_cond_wait(&count_cond, &count_lock);
      pthread_mutex_unlock(&count_lock);
      }
   else
      {
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

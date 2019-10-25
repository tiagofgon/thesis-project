// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File IoTask_P.C
//
// Demonstrates condition variable synchronization.
// The Pthreads idle wait protocol is used by main
// to launch a task and wait for its termination.
// - Main launches an IO task and waits on a
//   condition
// - The IO task performs its duty and signals the
//   condition
// -----------------------------------------------
#include <pthread.h>
#include <iostream>
#include <SPool.h>
#include <Timer.h>

bool  flag;
SPool TS(1);
pthread_mutex_t flock = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t fcond  = PTHREAD_COND_INITIALIZER;

// -------------------
// Worker threads code
// -------------------

void io_thread(void *idp)
   {
   Timer T;
   T.Wait(2000);     // perform IO
   std::cout << "\n IO task done. Signaling" << std::endl;

   // Toggle the predicate, and signal the change
   // -------------------------------------------
   pthread_mutex_lock(&flock);
   flag = false;
   pthread_mutex_unlock(&flock);
   pthread_cond_signal(&fcond);
   }   

// ------------------------
// Main, always the same...
// ------------------------
int main(int argc, char **argv)
   {
   flag = true;
   TS.Dispatch(io_thread, NULL);

   // ------- wait for false -----------
   pthread_mutex_lock(&flock);
   while(flag==true)
      pthread_cond_wait(&fcond, &flock);
   pthread_mutex_unlock(&flock);
   // ----------------------------------

   TS.WaitForIdle();
   return 0;
   }

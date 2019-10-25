// **************************************
// Copyright (c) 2011 Victor Alessandrini
// All rights reserved.
// **************************************
// TestBB3.C
// Third test of the blocking barrier class.
// Same as second test, but we replace the
// shutdown flag by a shutdown task.
//
// The purpose of this test is to show how to
// use the blocking barrier to implement the
// functionality of a SPMD thread pool (a
// pool where all the threads execute the
// same function).
//
// The ideas developped here are encapsulated
// in the SPool class.
//
// THIS CODE IS CORRECT
// ------------------------------------------

#include <iostream>
#include <BkBarrier.h>
#include <Timer.h>
#include <RandInt.h>
#include <Common.h>

pthread_t threads[16];        // this array hold the worker thread IDs
int rank[16];                 // this array holds thread ranks
BkBarrier *BB;                // the blocking barrier
RandInt r(1000);              // random generator, used by timer
bool shutdown = false;
void (*task)(void *p);        // pointer to task function

pthread_mutex_t  mylock = PTHREAD_MUTEX_INITIALIZER;   // protects stdout
pthread_mutex_t  mytask = PTHREAD_MUTEX_INITIALIZER;   // protects "task"

// --------------------------------------------
// This task function waits for a random period
// of time and then writes a message to stdout.
// The argument passed is the thread rank.
//
// This function is called by the thread function
// to execute a task. All threads execute the same
// task, posible differences are managed through
// the task rank.
// --------------------------------------------
void TaskFunction(void *p)
   {
   // Read the integer passsed via the (void*).
   // ---------------------------------------
   int *I = (int *)p;
   int rk = *I;

   // Wait 500 miliseconds plus something
   // ----------------------------------- 
   Timer T;
   T.Wait(500+r.draw());

   // Write to screen. Mutex is needed to prevent two threads
   // writing at the same time.
   // -------------------------------------------------------
   Pthread_Mutex_Lock(&mylock);
   std::cout << "\n Rank " << rk << " thread reporting" << std::endl;
   Pthread_Mutex_Unlock(&mylock);
   }


void ExitTask(void *P)
   {
   // Read the integer passsed via the (void*).
   // ---------------------------------------
   int *I = (int *)P;
   int rk = *I;

   // Write to screen. Mutex is needed to prevent two threads
   // writing at the same time.
   // -------------------------------------------------------
   Pthread_Mutex_Lock(&mylock);
   std::cout << "\n Rank " << rk << " thread exiting" << std::endl;
   Pthread_Mutex_Unlock(&mylock);
   pthread_exit(NULL);
   }

// -----------------------------------------------
// This is the thread function executed by all the
// worker threads
// -----------------------------------------------
void *ThFunction(void *P)
   {
   // Enter an infinite loop
   // ----------------------
   for(;;)
      {
      BB->Wait();             // Here worker threads sleep
      (*(task))(P);           // call the task function
      }
   }
    
// -------------------------------
// Release threads to perform a new
// parallel exetution
// -------------------------------        
void Dispatch(void (*TSK)(void *))             
   {
   BB->WaitForIdle();
   Pthread_Mutex_Lock(&mytask);
   task = TSK;
   Pthread_Mutex_Unlock(&mytask);
   BB->ReleaseThreads();
   }  

// -------------------------------------
// Set the shutdown flag, and launch the
// workers shutdown
// -------------------------------------
void RequestShutdown(int N)
   {
   Dispatch(ExitTask);
   for(int n=0; n<N; n++)
      pthread_join(threads[n], NULL);
   }


// ********
// MAIN
// ********
int main(int argc, char *argv[])
   {
   int n, nTh;
   Timer T;
   if(argc == 2) nTh = atoi(argv[1]); 
   else nTh = 2;
   std::cout << "\n Third test of BkBarrier class " << std::endl;

   BB = new BkBarrier(nTh);

   // Launch nTh identical threads
   // ----------------------------
   for(n=0; n<16; n++) rank[n] = n; 
   for(n=0; n<nTh; n++)
      pthread_create(&threads[n], NULL, ThFunction, (void *)&rank[n]);
   T.Wait(500);

   for(n=0; n<6; ++n)
      {
      Dispatch(TaskFunction);
      BB->WaitForIdle();
      std::cout << "\n ------------------------------" << std::endl;
      }

   RequestShutdown(nTh);
   delete BB;
   return 0;
   }




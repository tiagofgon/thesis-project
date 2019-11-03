// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// SimplePool_P.C
//
// The purpose of this example is to show how the blocking 
// barrier is used to implement the functionality of a SPMD 
// thread pool (a pool where all the threads execute the same 
// function).
//
// We use the native Pthreads interfaces for thread managment,
// in order to propose another full example of Pthreads programming.
//
// The ideas developped in this example are implemented in the 
// SPool class.
// --------------------------------------------------------

#include <iostream>
#include <BkBarrier.h>
#include <Timer.h>
#include <RandInt.h>
#include <Common.h>

pthread_t threads[16];        // this array hold the worker thread IDs
int rank[16];                 // this array holds thread ranks
BkBarrier *BB;                // the blocking barrier
RandInt r(1000);              // random generator, used by timer
bool active = true;
void (*task)(int k);          // pointer to task function

pthread_mutex_t  mylock = PTHREAD_MUTEX_INITIALIZER;   // protects stdout
pthread_mutex_t  mytask = PTHREAD_MUTEX_INITIALIZER;   // protects "task"

// -------------------------------------------------------------
// This task function waits for a random period of time and then 
// writes a message to stdout. The argument passed is the thread 
// rank.
//
// This function is not the thread function, it is a function called 
// by the thread function to execute a task. All threads execute the 
// same task, and posible differences are managed through the task 
// rank.
// --------------------------------------------------------------
void TaskFunction(int rk)
   {
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

// -----------------------------------------------
// This is the thread function executed by all the
// worker threads
// -----------------------------------------------
void *ThFunction(void *P)
   {
   // Read the integer passsed via the (void*).
   // ---------------------------------------
   int rk = *(int *)P;

   // Enter an infinite loop
   // ----------------------
   for(;;)
      {
      BB->Wait();                     // Here worker threads sleep
      if(active==true) (*(task))(rk);  // call the task function
      else break;
      }
   }
    
// -------------------------------
// Release threads to perform a new
// parallel exetution
// -------------------------------        
void Dispatch(void (*TSK)(int))             
   {
   BB->WaitForIdle();
   Pthread_Mutex_Lock(&mytask);
   task = TSK;
   Pthread_Mutex_Unlock(&mytask);
   BB->ReleaseThreads();
   }  

// --------------------------------------------------------
// Set the shutdown flag, and launch the workers shutdown. 
// After this call, the thread function breaks from the 
// infinite loop and terminates. Then, the worker threads
// are joined.
// --------------------------------------------------------
void RequestShutdown(int N)
   {
   active = false;
   BB->ReleaseThreads();
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

   BB = new BkBarrier(nTh);

   // Launch nTh identical threads
   // ----------------------------
   for(n=0; n<16; n++) rank[n] = n; 
   for(n=0; n<nTh; n++)
      pthread_create(&threads[n], NULL, ThFunction, (void *)&rank[n]);
   T.Wait(500);

   // Drive for 6 times the execution of the parallel tasks
   // -----------------------------------------------------
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




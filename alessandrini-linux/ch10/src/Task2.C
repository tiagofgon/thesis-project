// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File Task2.C
// ------------
//
// Checking how the task directive operate.
//
// The Pthread "pthread_t" thread identities are used to watch the
// task to thread mapping: which thread executes a given task in a 
// thread pool.
//
// This is a nice example. We start with a pool of 2 threads.
// - Thread 0 waits for a duration T=500ms - but this can be changed
//   from the command line.
// - Thread 1 waits for 1s and launches a child task. Since thread 0
//   is now available, the program shows that the spawned task is executed
//   by thread 0.
// - If, however, a duration T bigger than 1 second is passed from the
//   command line, thread 0 will not be available when the child task is
//   spawned, and the program shows that the task is executed by thread 1.
//
//
// Calibrating the Timer in threads 0 and 1, we can then see how the 
// task launched by thread 1 is scheduled.
// --------------------------------------------------------------

#include <iostream>
#include <omp.h>
#include <pthread.h>
#include <stdlib.h>
#include <Timer.h>

int nTh;
long main_wait;

// -------------------
// Auxiliary function
// -------------------
void Print_ID_Message(int R, int nTH)
    {
   #pragma omp critical
      {
      std::cout << "\nI am thread " << R << ", with " 
                << nTH << " threads in group, run by thread\n"
                << "Pth-ID " << pthread_self() << std::endl;
      }
   }
   
// ----------------------
// explicit task function
// ----------------------
void task1()
   {
   Timer T;
   int rank = omp_get_thread_num();
   T.Wait(1000);                // simulate some long treatment in task
   std::cout << "\n Spawned task, executed by thread " << rank
             << "\n Pthreads-ID " << pthread_self() << std::endl;
   }

// -----------------------
// Implicit task function
// -----------------------
void TaskFct()
   {
   Timer T;
   int rank = omp_get_thread_num();
   int nTH = omp_get_num_threads();
   Print_ID_Message(rank, nTH);

   if(rank==0)T.Wait(main_wait);     // make eventually thread 0 not available

   // ---------------------------------
   // Introduce an explicit task region
   // ---------------------------------

   if(rank==1)
      {
      T.Wait(1000);
      #pragma omp task  // spawn task, and continue
          {
          task1();
          }
      T.Wait(500);
      }
   }

// --------------------------
// This is the main function.
// --------------------------
int main(int argc, char**argv)
   {
   // get input
   // ---------
   nTh=2;
   main_wait = 500;
   if(argc == 2) main_wait = atoi(argv[1]);
    
   omp_set_num_threads(nTh);
   #pragma omp parallel
      {
      TaskFct();
      }
   }

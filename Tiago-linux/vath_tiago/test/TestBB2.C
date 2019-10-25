// **************************************
// Copyright (c) 2011 Victor Alessandrini
// All rights reserved.
// **************************************
// TestBB2.C
// Second test of the blocking barrier class.
//
// The purpose of this test is to show how to
// use the blocking barrier to implement the
// functionality of a SPMD thread pool (a
// pool where all the threads execute the
// same function).
//
// The ideas developped here are encapsulated
// in the SPool class.
// ------------------------------------------

#include <iostream>
#include <BkBarrier.h>
#include <Timer.h>
#include <random>
#include <thread>
#include <mutex>

pthread_t threads[16];        // this array hold the worker thread IDs
int rank[16];                 // this array holds thread ranks
BkBarrier *BB;                // the blocking barrier
std::uniform_int_distribution<unsigned> r(0, 1000);  // random generator, used by timer
std::default_random_engine e;
bool shutdown = false;
void (*task)(void *p);        // pointer to task function
std::vector<std::thread> workers;

std::mutex  mylock;   // protects stdout
std::mutex  mytask;   // protects "task"

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
void TaskFunction(int rk)
   {
   // Wait 500 miliseconds plus something
   // ----------------------------------- 
   Timer T;
   T.Wait(500 + r(e));

   // Write to screen. Mutex is needed to prevent two threads
   // writing at the same time.
   // -------------------------------------------------------
       {
	   std::lock_guard<std::mutex> lock(mylock);
	   std::cout << "\n Rank " << rk << " thread reporting" << std::endl;
       }
   }


// -----------------------------------------------
// This is the thread function executed by all the
// worker threads
// -----------------------------------------------
void ThFunction(int rk)
   {
   int myrank;
   bool exit;
   myrank = rk;
 
   // Enter an infinite loop
   // ----------------------
   for(;;)
      {
      BB->Wait();        // Here worker threads sleep
      
      // Worker threads are waken up by client thread
      // --------------------------------------------
         {
         std::lock_guard<std::mutex> lock(mytask);
         exit = shutdown;
         }

      if(exit==true)         // if shutdown, exit
          {
             { 
             std::lock_guard<std::mutex> lock(mylock);
             std::cout << "\n Rank " << myrank << " thread exits" << std::endl;
             }
          break;
          }
      else TaskFunction(myrank);            // else call the task function
      }
   }
    
// -------------------------------
// Release threads to perform a new
// parallel exetution
// -------------------------------        
void Dispatch()             
   {
   BB->ReleaseThreads();
   }  

// -------------------------------------
// Set the shutdown flag, and launch the
// workers shutdown
// -------------------------------------
void RequestShutdown(int N)
   {
       {
       std::lock_guard<std::mutex> lock(mytask);
       shutdown = true;
       }
   Dispatch();
   for(auto &th : workers) th.join();
   }

int main(int argc, char *argv[])
   {
   int n, nTh;
   Timer T;
   if(argc == 2) nTh = atoi(argv[1]); 
   else nTh = 2;

   BB = new BkBarrier(nTh);
   std::cout << "\n *** Second test of BkBarrier class\n " 
             << std::endl;

   // Launch nTh identical threads
   // ----------------------------
   for (n=0; n<nTh; ++n)
       workers.push_back(std::thread(ThFunction, rank[n]));
   T.Wait(500);

   for(n=0; n<6; ++n)
      {
      Dispatch();
      BB->WaitForIdle();
      std::cout << "\n ------------------------------" << std::endl;
      }
   RequestShutdown(nTh);
   std::cout << "Main: done" << std::endl;

   delete BB;
   return 0;
   }




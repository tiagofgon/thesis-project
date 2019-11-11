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
#include <thread>
#include <vector>
#include <BkBarrier.h>
#include <Timer.h>
#include <RandInt.h>
#include <SafeCout.h>

std::vector<std::thread> workers;
BkBarrier *BB;                // the blocking barrier
RandInt r(1000);              // random generator used by timer
bool shutdown = false;
SafeCout  SC;

void (*task)(int n);        // pointer to task function
std::mutex  mytask;         // protects "task"

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
   std::ostringstream os;

   // Wait 500 miliseconds plus something
   // ----------------------------------- 
   Timer T;
   T.Wait(500+r.draw());

   // Write to screen. Mutex is needed to prevent two threads
   // writing at the same time.
   // -------------------------------------------------------
   os << "\n Rank " << rk << " thread reporting";
   SC.Flush(os);
   }


void ExitTask(int rk)
   {
   std::ostringstream os;

   // Write to screen. 
   // ----------------
   os << "\n Rank " << rk << " thread exiting";
   SC.Flush(os);
   pthread_exit(NULL);
   }

// -----------------------------------------------
// This is the thread function executed by all the
// worker threads
// -----------------------------------------------
void ThFunction(int n)
   {
   // Enter an infinite loop
   // ----------------------
   for(;;)
      {
      BB->Wait();             // Here worker threads sleep
      if(shutdown) break;
      else (*(task))(n);      // call the task function
      }
   }
    
// -------------------------------
// Release threads to perform a new
// parallel exetution
// -------------------------------        
void Dispatch(void (*TSK)(int))             
   {
   BB->WaitForIdle();
      {
      std::lock_guard<std::mutex> lg(mytask);
      task = TSK;
      }
   BB->ReleaseThreads();
   }  

// -------------------------------------
// Set the shutdown flag, and launch the
// workers shutdown
// -------------------------------------
void RequestShutdown()
   {
   shutdown = true;
   BB->ReleaseThreads();

   // Join the terminating threads
   // ----------------------------
   for(auto &th : workers) th.join();
   }


// ********
// MAIN
// ********
int main(int argc, char *argv[])
   {
   int n, nTh;
   Timer T;
   int rank[16];

   if(argc == 2) nTh = atoi(argv[1]); 
   else nTh = 2;

   BB = new BkBarrier(nTh);
   std::cout << "\n *** Test of the SPool class architecture\n " 
             << std::endl;

   // Launch nTh identical threads
   // ----------------------------
   for(n=0; n<16; n++) rank[n] = n; 

   for (n=0; n<nTh; ++n)
       workers.push_back(std::thread(ThFunction, rank[n]));
   T.Wait(500);

   for(n=0; n<6; ++n)
      {
      Dispatch(TaskFunction);
      BB->WaitForIdle();
      std::cout << "\n ------------------------------" << std::endl;
      }

   RequestShutdown();
   delete BB;
   return 0;
   }




// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ***************************************** 
//
// File IoTask.C
//
// Demonstrates condition variable synchronization.
// The CPP11 idle wait protocol is used by main
// to launch a task and wait for its termination.
// - Main launches an IO task and waits on a
//   condition
// - The IO task performs its duty and signals the
//   condition

// -----------------------------------------------
#include <iostream>
#include <SPool.h>
#include <Timer.h>
#include <mutex>
#include <condition_variable>

bool  flag;
SPool TS(1);
std::mutex              fmutex; 
std::condition_variable fcond;

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
      {
      std::unique_lock<std::mutex> lock(fmutex);
      flag = false;
      }
   fcond.notify_one();
   }   

// ------------------------
// Main, always the same...
// ------------------------
int main(int argc, char **argv)
   {
   flag = true;
   TS.Dispatch(io_thread, NULL);

   // ------- wait for false -----------
      {
      std::unique_lock<std::mutex> lock(fmutex);
      while(flag==true)
         fcond.wait(lock);
      }
   TS.WaitForIdle();
   return 0;
   }

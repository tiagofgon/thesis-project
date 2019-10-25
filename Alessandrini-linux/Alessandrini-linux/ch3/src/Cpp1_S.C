// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// Cpp1_S.C : first C++11 example
// 
// This example shows how a thread is launched by
// main(). Then, main() waits for the termination
// by joining the worker threads.
//
// The worker thread function sleeps or a number of 
// milliseconds, and writes a message. The sleep 
// duration is passed as an argument to the thread 
// function.
//
// main() constructs the thread as a local object,
// and then joins the thread, waiting for termination.
// -------------------------------------------------

#include <iostream>
#include <thread>
#include <chrono>

void ThreadFunc(unsigned mSecs)
   {
   // create a time duration (mSecs) object
   // -------------------------------------
   std::chrono::milliseconds workTime(mSecs);
   std::cout << "Worker running. Sleeping for " 
             << mSecs << std::endl;

   // pretend to do something useful
   // ------------------------------
   std::this_thread::sleep_for(workTime);
   std::cout << "worker: finished" << std::endl;
   }

// --------------------------------------------------------
// This is the simples possible scenario, having a C-style
// function that we want to run as a separate thread
// --------------------------------------------------------
int main(int argc, char* argv[])
   {
   std::cout << "main: startup" << std::endl;

   std::thread T(ThreadFunc, 3000);  // thread created
   std::cout << "main: waiting for thread " << std::endl;
   T.join();
   std::cout << "main: done" << std::endl;
   return 0;
   }



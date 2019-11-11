// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// Cpp2_S.C : second C++11 example
// 
// Same as Cpp1_S.C, but now the new thread is a global
// object, allocated in the heap. This is needed when 
// the new thread must be seen by threads other than
// main(), that has created it.
// -------------------------------------------------

#include <iostream>
#include <thread>
#include <chrono>

void ThreadFunc(unsigned mSecs)
   {
   // create a time duration (mSecs) object
   std::chrono::milliseconds workTime(mSecs);
   std::cout << "Worker running. Sleeping for " 
             << mSecs << std::endl;

   // pretend to do something useful
   std::this_thread::sleep_for(workTime);
   std::cout << "worker: finished" << std::endl;
   }

// -----------
// Global data
// -----------
std::thread *Wth;

// --------------------------------------------------------
// This is the simples scenario, when we have a C-style
// function that we want to run as a separate thread
// --------------------------------------------------------
int main(int argc, char* argv[])
   {
   std::cout << "main: startup" << std::endl;

   // here, thread is created and launched
   Wth = new std::thread(ThreadFunc, 3000);

   std::cout << "main: waiting for thread " << std::endl;
   Wth->join();
   std::cout << "main: done" << std::endl;
   delete Wth;
   return 0;
   }



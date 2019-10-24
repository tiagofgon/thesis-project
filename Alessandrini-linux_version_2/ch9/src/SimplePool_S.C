// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// SimplePool_S.C

// The purpose of this example is to show how the blocking 
// barrier is used to implement the functionality of a SPMD 
// thread pool (a pool where all the threads execute the same 
// function).
//
// We use the native C++11 interfaces for thread managment,
// to propose another full example of C++11 programming.
//
// The ideas developped in this example are implemented in 
// the SPool class.
// --------------------------------------------------------

#include <iostream>
#include <thread>
#include <vector>
#include <BkBarrier.h>
#include <Timer.h>
#include <RandInt.h>
#include <SafeCout.h>

std::vector<std::thread> W;   // this array hold the worker threads
int *rank;                    // this array holds thread ranks
BkBarrier *BB;                // the blocking barrier
RandInt r(1000);              // random generator, used by timer
bool active = true;
void (*task)(int n);          // pointer to task function

std::mutex  mytask;           // protects "task"
SafeCout    SC;               // ordered output to stdout

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
   std::ostringstream os;
   Timer T;
  
   // Wait, and write to screen 
   T.Wait(500+r.draw());      
   os << "\n Rank " << rk << " thread reporting";
   SC.Flush(os);
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
      BB->Wait();                // Here worker threads sleep
      if(active) (*(task))(n);   // call the task function
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
      {
      std::lock_guard<std::mutex> mylock(mytask);
      task = TSK;
      }
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
   for(auto &th : W) th.join();
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

   rank = new int[nTh];
   BB = new BkBarrier(nTh);

   // Launch nTh identical threads
   // ----------------------------
   for(n=0; n<nTh; n++) rank[n] = n; 
   for(n=0; n<nTh; n++) W.push_back(std::thread(ThFunction, rank[n])); 
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




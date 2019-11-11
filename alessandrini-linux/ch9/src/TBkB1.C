// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// TBkB1.C
//
// Testing the blocking barrier. Introducing a general purpose 
// thread function that executes tasks between two blocks.
//
// Worker threads are launched, they enter an infinite for loop and
// hit a blocking barrier, where they wait to be released. When 
// released, they call a task function via a function pointer, 
// and block again at the barrier. The main function successively
// releases them by proposing tasks to execute (in this case, always
// the same task for simplicity).
//
// Look at the way the worker threads are finally released from the
// infinite loop, and terminate. Look at the simplicity of this code
// setup, arinsing from the blocking barrier features.
// ---------------------------------------------------------------

#include <iostream>
#include <BkBarrier.h>
#include <Timer.h>
#include <RandInt.h>
#include <SPool.h>

BkBarrier *BB;
SPool     *TS;
bool      active;

// --------------------------------------------------------------
// RandInt object:
// --------------
// r.draw() returns a random integer in [0, 1000]. This is used
// by the Timer to implement a random wait of less than 1 second
// -------------------------------------------------------------
RandInt r(1000);

void (*task)();      // pointer to function with no argument and
                     // no return value

// This is a task function called by the thread function
// -----------------------------------------------------
void taskfct()
   {
   int rank = TS->GetRank();
   Timer T;
   T.Wait(500+r.draw());
   std::cout << "\n Rank " << rank << " thread reporting" << std::endl;
   }


void thread_fct(void *p)
   {
   for(;;)
      {
      BB->Wait();
      if(active==true) (*task)();
      else break;
      }
   }


int main(int argc, char *argv[])
   {
   int n, nTh;
   Timer T;
   if(argc == 2) nTh = atoi(argv[1]); 
   else nTh = 2;
   active = true;

   BB = new BkBarrier(nTh);
   TS = new SPool(nTh);
   TS->Dispatch(thread_fct, NULL);         // launch worker threads

   T.Wait(500);
   
   task = taskfct;               // initialize pointer to function
   for(n=0; n<4; ++n)
      {
      BB->ReleaseThreads();
      BB->WaitForIdle();
      std::cout << "\n ------------------------------" << std::endl;
      }

   active = false;            //prepare break from infinite loop
   BB->ReleaseThreads();
   TS->WaitForIdle();
   delete BB;
   delete TS;
   return 0;
   }




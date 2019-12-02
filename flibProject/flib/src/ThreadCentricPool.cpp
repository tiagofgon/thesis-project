/* ---------------------------------------------------------------------------
                           Made by Tiago Gonçalves - 2019
 --------------------------------------------------------------------------*/
/* ThreadCentricPool.C
 * Implementation code for SPMD thread pool.
 ********************************************/
#include "ThreadCentricPool.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <utility>      // std::pair
#include <iostream>

using namespace std;

// ************************************************
// This is the thread function that will be passed 
// to _beginthreadex(). IT IS NOT a class member. 
// It wraps the "true" thread function that will be 
// executed by worker threads in pool.**
// ************************************************

void threadFunc(void *P)
   {
   ThreadCentricPool *tp = (ThreadCentricPool *)P;
   tp->PeerThread();  // call member function in pool
   }


// **************************
// Constructor and destructor
// **************************

ThreadCentricPool::ThreadCentricPool(int nTh, double stksize)
   {
   int n;

   // initialize the fields, copying the input parameters 
   // ---------------------------------------------------
   nThreads = nTh;
   shut = false;

   // allocate memory for pthread_t identifiers
   // ------------------------------------------
   WT       = new std::thread*[nThreads+1];
   th_id    = new std::thread::id[nTh+1];

   // initialize CS and blocking barrier.
   // ---------------------------------------
   BlBarrier = new BkBarrier(nTh);

   // Launch threads
   // --------------
   for(n=1; n<=nThreads; n++)
      {
      WT[n] = new std::thread(threadFunc, (void*)this) ;
      th_id[n] = WT[n]->get_id();
      }
   }

ThreadCentricPool::~ThreadCentricPool()
   {
   if(shut==false) JoinThreads();

   // Release memory
   // --------------
   for(int n=1; n<=nThreads; n++) delete WT[n];
   delete [] th_id;
   delete BlBarrier;
   }

// ----------------------------------------------
// Function executed by the internal peer threads
// ----------------------------------------------
void ThreadCentricPool::PeerThread()
   {
   int rank = GetRank();
   bool my_shut;
   for(;;)
       {
       BlBarrier->Wait();  // here worker thread sleeps
          {
          std::lock_guard<std::mutex> lock(pMutex);
          my_shut = shut;
          }
       if(my_shut == false) (*(fct))(arg);
          /*
	  {           // call task function under try-catch block
	  try { (*(fct))(arg); }
	  catch (std::runtime_error) {}
	  }
          */
       else break;
       }
   }

// --------------------------------------------------
// Function called by manager thread to dispatch work
// --------------------------------------------------
void ThreadCentricPool::Dispatch(void (*funct)(void*), void *argm)
   {
   BlBarrier->WaitForIdle();

   // Reinitialize request data
   // -------------------------
       {
       std::lock_guard<std::mutex> lock(pMutex);
       fct = funct;
       arg = argm;
       }
   // Release the worker threads
   // --------------------------
   BlBarrier->ReleaseThreads();
   }


void ThreadCentricPool::WaitForIdle()
   {
   BlBarrier->WaitForIdle();
   }

// ------------------------------------------------------------
// This function will be called by worker threads if they need
// to know the rank of the thread that is executing the job.
// This is important to distribute work among worker thread.
// Ranks are in [1, nTh].
// ------------------------------------------------------------
int ThreadCentricPool::GetRank()
   {
   std::thread::id my_id; 
   int n, my_rank;

   my_id = std::this_thread::get_id();          // determine who  am
       {
       std::lock_guard<std::mutex> lock(pMutex);
       n = 0;
       do
          {
          n++;
          } while(my_id != th_id[n] && n<nThreads); 
       }

   if(n<=nThreads) my_rank = n;    // OK, return rank
   else my_rank = 0;     // else, return error
   return my_rank;
   }

// ----------------------------------------------------
// These functions receives as argument the global
// range (STL conventions) and returns in the arguments 
// the thread range. Internally, it determines the rank 
// of the calling thread, to compute the thread range.
//
// Ranges are [beg, end) : for integers, end is index 
// to past the last end element
// -----------------------------------------------
// void ThreadCentricPool::ThreadRange(int& Beg, int& End)
//    {
//    int n, rank, beg, end;
//    int size, D, R;
//    rank = GetRank();

//    size = End-Beg;
//    D = (size/nThreads);
//    R = size%nThreads;

//    end = Beg;
//    for(n=1; n<=rank; n++)
//       {
//       beg = end;
//       end = beg+D;
//       if(R)
//          {
//          end++;
//          R--;
//          }
//       }
//    Beg = beg;
//    End = end;
//    }

std::pair<int,int> ThreadCentricPool::schedule_static(int Beg, int End) {
   int n, rank, beg, end;
   int size, D, R;
   rank = GetRank();

   size = End-Beg;
   D = (size/nThreads);
   R = size%nThreads;

   end = Beg;
   for(n=1; n<=rank; n++)
      {
      beg = end;
      end = beg+D;
      if(R)
         {
         end++;
         R--;
         }
      }
   Beg = beg;
   End = end;
   return std::make_pair (Beg,End);
}

std::pair<int,int> ThreadCentricPool::schedule_dynamic(int Beg, int End)
   {
   int n, rank, beg, end;
   int size, D, R;
   rank = GetRank();

   size = End-Beg;
   D = (size/nThreads);
   R = size%nThreads;

   end = Beg;
   for(n=1; n<=rank; n++)
      {
      beg = end;
      end = beg+D;
      if(R)
         {
         end++;
         R--;
         }
      }
   Beg = beg;
   End = end;
   return std::make_pair (Beg,End);
   }

std::pair<int,int> ThreadCentricPool::schedule_guided(int Beg, int End)
   {
   int n, rank, beg, end;
   int size, D, R;
   rank = GetRank();

   size = End-Beg;
   D = (size/nThreads);
   R = size%nThreads;

   end = Beg;
   for(n=1; n<=rank; n++)
      {
      beg = end;
      end = beg+D;
      if(R)
         {
         end++;
         R--;
         }
      }
   Beg = beg;
   End = end;
   return std::make_pair (Beg,End);
   }

// void ThreadCentricPool::ThreadRange(double& Beg, double& End)
//    {
//    int rank;
//    double beg, end;
//    double size, D;

//    rank = GetRank();
//    size = End-Beg;
//    D = size/nThreads;
//    beg = Beg + (rank-1)*D;
//    end = Beg + rank*D;
   
//    Beg = beg;
//    End = end;
//    }

// -------------------------------------------
// This function is called by one of the worker
// threads to cancel all the other workers in
// the team
// --------------------------------------------
void ThreadCentricPool::CancelTeam()
   {
   std::cout << "\n Cancellation not yet implemented on C++11 on Linux"
             << std::endl;
   exit(0);
   /*
   // Set cancellation flag
   cancel_flag.store(true, std::memory_order_relaxed);

   // Now, interrupt this task
   // --------------------------
   // SetActive(false);
   throw std::runtime_error("Task interrupted");
   */
   }

void ThreadCentricPool::SetCancellationPoint()
   {
   std::cout << "\n Cancellation not yet implemented on C++11 on Linux"
             << std::endl;
   exit(0);

   // if (cancel_flag.load(std::memory_order_relaxed))
   //    throw std::runtime_error("Task interrupted");
   }


void ThreadCentricPool::JoinThreads()
   {
   // set the shut flag
   // ------------------
      {
      std::lock_guard<std::mutex> lock(pMutex);
      shut = true;
      }

   BlBarrier->WaitForIdle(); 
   BlBarrier->ReleaseThreads();
   // Wait for threads to exit
   // ------------------------
   for(int n=1; n<=nThreads; n++) WT[n]->join();
   }

//*******************************************************

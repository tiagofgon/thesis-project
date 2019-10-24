/* ---------------------------------------------------------------------------
    Copyright 2014-2015 Victor Alessandrini.  All Rights Reserved.

    This file is part of the software support provided wih the book
    "Shared Mamory Application Programming".

    This code is free software; you can redistribute it and/or modify it under 
    the terms of the GNU General Public License version 2 as published by 
    the Free Software Foundation.

    This software is distributed in the hope that it will be useful, but 
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
    for more details.

    You should have received a copy of the GNU General Public License
    along with Threading Building Blocks; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
 --------------------------------------------------------------------------*/
// ThreadMgr.C. 
//
//----------
#include <iostream>
#include <thread>
#include <stdlib.h>
#include "ThreadMgr.h"
#include <mutex>
#include <vector>



// ***************< THREAD FUNCTION >**********************
// This function is the thread function that will be passed 
// to pthreads_create. IT IS NOT a member function of the 
// NPool class. It calls the "true" thread function that 
// will be performed by the threads, and which is a member
// function of the NPool class (so that it can access its
// internal data items).
//
// This function receives as argument the address of the
// NPool object that implements the thread pool.
// -------------------------------------------------------

extern void ThreadFunction(void *arg, int n);

// ====================
// Class ThreadManager 
// ====================

// Constructor
// -----------
ThreadMgr::ThreadMgr(int nTh, void *P)
   {
   nThreads = nTh;

   // allocate memory for thread and task identifiers
   ///--------------------------------------------------
   WT       = new std::thread*[nThreads+1];
   curr_task = new Task*[nThreads+1];

   // Create the worker threads 
   // -------------------------

   for(int n=1; n<=nThreads; n++)
      {
      WT[n] = new std::thread(ThreadFunction, P, n);
      }

   //std::chrono::milliseconds ms(100);
   //std::this_thread::sleep_for(ms);
   }

// -------------------------------------
// Destructor
// -------------------------------------
ThreadMgr::~ThreadMgr()
   {
   // Wait for threads to exit
   // ------------------------
   for(int n=1; n<=nThreads; n++) WT[n]->join();
   
   // Release memory
   // --------------
   delete [] curr_task;
   for(int n=1; n<=nThreads; ++n) delete WT[n];
   delete [] WT;
   }


// -----------------------------------------------------------
// GetRank().
// ---------
// This member function returns the rank of the caller thread.
// Ranks of worker threads are in [1, nThreads]. If the caller
// thread is not a worker thread, the rank (-1) is returned.
// -----------------------------------------------------------
int ThreadMgr::GetRank()
   {
   std::thread::id my_id, target_id; 
   int n, my_rank;

   my_id = std::this_thread::get_id();    // determine who  am
       {
	   std::lock_guard<std::mutex> lock(mgr_mutex);
         n = 0;
         my_rank = -1;
         for (n = 1; n <= nThreads; ++n) {
            target_id = WT[n]->get_id();
            if (target_id == my_id) {
               my_rank = n;
               break;
            }
         }
       }
   return my_rank;
   }


// -------------------------------------------------------
// SetCurrTask()
// -------------
// This member function establishes the mapping of a task 
// to an executing thread. It will insert the Task* t at
// curr_task[n], where n is the rank of the calling thread
// ------------------------------------------------------- 
void ThreadMgr::SetCurrTask(Task *t)
   {
   int rank = GetRank();
   std::lock_guard<std::mutex> lock(mgr_mutex);
   curr_task[rank] = t;
   } 


// --------------------------------------------------------
// GetCurrTask()
// -------------
// This member function returns the Task* of the task being
// currently executed by the caller thread
// --------------------------------------------------------
Task* ThreadMgr::GetCurrTask()
   {
   Task *t;
   int rank = GetRank();
      {
      std::lock_guard<std::mutex> lock(mgr_mutex);
      t = curr_task[rank];
      }
   return t;
   } 

//******************************************************

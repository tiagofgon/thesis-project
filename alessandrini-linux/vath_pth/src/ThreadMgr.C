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

// ThreadMgr.C
//-------------

#include <stdlib.h>
#include <ThreadMgr.h>
#include <errors.h>
#include <Common.h>


// ***************< THREAD FUNCTION >**********************
// This function is the thread function that will be passed 
// to pthreads_create. IT IS NOT a member function of the 
// NPool class. It calls the "true" thread function that 
// will be performed by the threads, and which is a member
// function of the NPool class (so that it can access its
// internal data items).
//
// This function receives as argument the address of the
// NPool object that implements the thread pool. It is
// defined in NPool.C
// -------------------------------------------------------

extern void *ThreadFunction(void *arg);

// Constructor
// -----------
ThreadManager::ThreadManager(int nTh, void *P)
   {
   nThreads = nTh;

   // --------------------------------------------------
   // allocate memory for pthread_t and task identifiers
   ///--------------------------------------------------
   threads = (pthread_t *)malloc(sizeof(pthread_t)*(nTh+1));
   if(threads == NULL) errno_abort("Malloc threads");

   curr_task  = (Task**) malloc(sizeof(Task*)*(nTh+1));
   if(curr_task == NULL) errno_abort("malloc curr_task");
   
   // -----------------------------------------------------
   // Initialize the threads attribute. All threads in the 
   // pool share, of course, the same attributes. They are 
   // forced to be joinable (in case we are running AIX). 
   // They are system threads. 
   // -----------------------------------------------------
   pthread_attr_init( &attr);
   pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE);
   pthread_attr_setscope( &attr, PTHREAD_SCOPE_SYSTEM);

   // -----------------
   // initialize mutex
   // -----------------
   int status = pthread_mutex_init( &mgr_mutex, NULL);
   if(status != 0) err_abort(status, "Mutex Init");

   // -------------------------
   // Create the worker threads 
   // -------------------------
   for (int n=1; n<=nThreads; n++) 
      {
      int status = pthread_create(&threads[n], &attr,
			      ThreadFunction, P);
      if(status != 0) err_abort(status, "Thread creation"); 
      }
   }

// Destructor
// ----------
ThreadManager::~ThreadManager()
   {
   int status;
   for (int n=1; n<=nThreads; n++) 
      {
      status = pthread_join(threads[n], NULL);
      if(status != 0) err_abort(status, "Thread Join"); 
      } 
   pthread_mutex_destroy(&mgr_mutex);
   free(threads);
   free(curr_task);
   }

// -----------------------------------------------------------
// This member function returns the rank of the caller thread.
// Ranks of worker threads are in [1, nThreads]. If the caller
// thread is not w worker thread, the rank 0 is returned.
// -----------------------------------------------------------
int ThreadManager::GetRank()
   {
   pthread_t my_id;
   int n, my_rank, retval, status;

   // Thread tries to determine its rank
   // ----------------------------------
   my_id = pthread_self();
   Pthread_Mutex_LockBis( &mgr_mutex, "GetRank()");
   // -----------------------------------------
   n = 0;
   do
      {
      n++;
      status = pthread_equal(my_id, threads[n]);
      } while(status==0 && n < nThreads);        // NOTICE !! 
   // -----------------------------------------
   Pthread_Mutex_Unlock( &mgr_mutex);
 
   if(status) my_rank=n;
   else my_rank = 0;
   return my_rank;
   }

// -------------------------------------------------------
// This member function establishes the mapping of a task 
// to an executing thread. It will insert the Task* t at
// curr_task[n], where n is the rank of the calling thread
// ------------------------------------------------------- 
void ThreadManager::SetCurrTask(Task *t)
   {
   int rank = GetRank();
   Pthread_Mutex_LockBis(&mgr_mutex, "SetCurrTask()");
   curr_task[rank] = t;
   Pthread_Mutex_Unlock(&mgr_mutex);
   } 

// --------------------------------------------------------
// This member function returns the Task* of the task being
// currently executed by the caller thread
// --------------------------------------------------------
Task* ThreadManager::GetCurrTask()
   {
   int rank = GetRank();
   Pthread_Mutex_LockBis(&mgr_mutex, "GetCurrTask()");
   Task *t = curr_task[rank];
   Pthread_Mutex_Unlock(&mgr_mutex);
   return t;
   } 


//******************************************************

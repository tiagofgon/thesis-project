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

/********************************************
 * SPool.C
 * Implementation code for SPMD thread pool.
 ********************************************/
#include <SPool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errors.h>
#include <Common.h>
#include <iostream>

// ******************************************
// This is the thread function that will be 
// passed to pthreads_create. IT IS NOT a
// class member. It wraps the "true" thread 
// function that will be executed by threads.
// ******************************************

void *ThreadFunc(void *P)
   {
   SPool *tp = (SPool *)P;
   tp->PeerThread();  // call the member function of the caller pool
   return NULL;
   }


// **************************
// Constructor and destructor
// **************************

SPool::SPool(int nTh, double stksize)
   {
   int n, status;

   // initialize the fields, copying the input parameters 
   // ---------------------------------------------------
   nThreads = nTh;
   active = true;

   // allocate memory for pthread_t identifiers
   // ------------------------------------------
   threads = (pthread_t *)malloc(sizeof(pthread_t)*(nThreads+1)); 
   if(threads == NULL) errno_abort("Malloc threads"); 

   // Initialize the threads attribute. All threads in the 
   // pool share, of course, the same attributes. They are 
   // joinable.
   // -----------------------------------------------------
   pthread_attr_init( &attr);
   pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE);
   pthread_attr_setscope( &attr, PTHREAD_SCOPE_SYSTEM);


   // If required, change the stack size
   // ----------------------------------
   if(stksize != 0.0)
      {
      size_t size;
      pthread_attr_getstacksize(&attr, &size);
      pthread_attr_setstacksize(&attr, stksize*size);
      }

   // initialize mutex and blocking barrier.
   // ---------------------------------------
   status = pthread_mutex_init( &qLock, NULL);
   if(status != 0) err_abort(status, "Mutex init"); 
   BlBarrier = new BkBarrier(nTh);

   // Launch the worker threads 
   // -------------------------
   for (n=1; n<=nThreads; n++) 
      {
      status = pthread_create(&threads[n], &attr,
			      ThreadFunc, (void *)this);
      if(status != 0) err_abort(status, "Thread create");
      }
   }

SPool::~SPool()
   {
   int n, status;
   bool state;

   // Next, we join the terminating threads
   // if they are still active (not previously
   // cancelled)
   // -------------------------------------
   state = GetActive();
   if(state)
      {
      SetActive(false);
      BlBarrier->WaitForIdle();
      BlBarrier->ReleaseThreads();
      }

   // Join the terminating threads
   // ----------------------------
   for(n=1; n<=nThreads; n++)
      {
      status = pthread_join(threads[n], NULL);
      if(status) err_abort(status, "Thread join");
      }
  
   // Release memory
   // --------------
   free(threads);
   delete BlBarrier;
   }

// ----------------------------------------------
// Function executed by the internal peer threads
// ----------------------------------------------
void SPool::PeerThread()
   {
   bool state;
   for(;;)
       {
       BlBarrier->Wait();  // here worker thread sleeps
       state = GetActive();
       if(state) (*(fct))(arg);  // when released, calls work function 
       else break;
       } 
   }

// -------------------------------------
// Getting and setting the active status
// -------------------------------------
bool SPool::GetActive()
   {
   bool retval;
   Pthread_Mutex_Lock(&qLock);
   retval = active; 
   Pthread_Mutex_Unlock(&qLock);
   return retval;
   }
 
void SPool::SetActive(bool state)
   {
   Pthread_Mutex_Lock(&qLock);
   active = state; 
   Pthread_Mutex_Unlock(&qLock);
   }
 
// --------------------------------------------------
// Function called by manager thread to dispatch work
// --------------------------------------------------
void SPool::Dispatch(void (*funct)(void *), void *argm)
   {
   BlBarrier->WaitForIdle();

   // Reinitialize request data
   // -------------------------
   Pthread_Mutex_Lock(&qLock);
   fct = funct;
   arg = argm;
   Pthread_Mutex_Unlock(&qLock);

   // Release the worker threads
   // --------------------------
   BlBarrier->ReleaseThreads();
   }


void SPool::WaitForIdle()
   {
   BlBarrier->WaitForIdle();
   }

// ------------------------------------------------------------
// This function will be called by worker threads if they need
// to know the rank of the thread that is executing the job.
// This is important to distribute work among worker threads.
// ------------------------------------------------------------
int SPool::GetRank()
   {
   pthread_t my_id;
   int n, my_rank, status;

   my_id = pthread_self();          // determine who  am
   
   Pthread_Mutex_Lock( &qLock);
   n = 0;
   do
      {
      n++;
      status = pthread_equal(my_id, threads[n]);
      } while(status==0 && n < nThreads); 
   Pthread_Mutex_Unlock( &qLock);
 
   if(status) my_rank=n;    // OK, return rank
   else my_rank = (-1);     // else, return error
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
void SPool::ThreadRange(int& Beg, int& End)
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
   }

void SPool::ThreadRange(double& Beg, double& End)
   {
   int n, rank;
   double beg, end;
   double size, D;

   rank = GetRank();
   size = End-Beg;
   D = size/nThreads;
   beg = Beg + (rank-1)*D;
   end = Beg + rank*D;
   
   Beg = beg;
   End = end;
   }

// -------------------------------------------
// This function is called by one of the worker
// threads to cancel all the other workers in
// the team
// --------------------------------------------
void SPool::CancelTeam()
   {
   int k, status;
   int rank = GetRank();
   pthread_t my_id = pthread_self();     // ID of calling thread

   for(k=1; k<=nThreads; k++)
      {
       status = pthread_equal(my_id, threads[k]);
       if(status==0) pthread_cancel(threads[k]);
      }

   SetActive(false);           // team is no longer active
   BlBarrier->WakeUpClient();  // client stops waiting for idle
   pthread_exit(NULL);         // exit after cancelling the other threads
   }

void SPool::SetCancellationPoint()
   {
   pthread_testcancel();
   }

// ------------------------------------------
// This function is called by client treads
// It replaces WaitForIdle when the submitted
// job leans to the cancellation of the worker
// team
// -------------------------------------------
void SPool::Join()
   {
   int n, status;
   bool state;

   //BL.Wait_Until_True(0);
   for(n=1; n<=nThreads; n++)
       {
       status = pthread_join(threads[n], NULL);
       if(status) err_abort(status, "Thread join");
       }
   }

//*******************************************************

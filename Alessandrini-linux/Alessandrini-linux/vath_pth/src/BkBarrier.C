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

// BkBarrier.C
// This file implements the Barrier class
// adapted to SPool needs
// ---------------------------------------*/

#include <pthread.h>
#include <BkBarrier.h>
#include <errors.h>

// Constructor
// -----------
BkBarrier::BkBarrier(int count)
   {
   int status;

   // Initialize fields and Pthreads structures 
   
   nTh = counter = count;
   cycle = true;
   is_idle = false;
   status = pthread_mutex_init (&mutex, NULL);
   if (status != 0) err_abort(status, "pthread_mutex_init");
   status = pthread_cond_init (&qtask, NULL);
   if (status != 0) err_abort(status, "pthread_cond_init");
   status = pthread_cond_init (&qidle, NULL);
   if (status != 0) err_abort(status, "pthread_cond_init");
   }
 
// Destructor.
// ----------
BkBarrier::~BkBarrier ()
   {
   int status;
   status = pthread_mutex_destroy (&mutex);
   status = pthread_cond_destroy (&qtask);
   status = pthread_cond_destroy (&qidle);
   }

/* ------------------------------------------------------------
 * Wait for all members of a barrier to reach the barrier. When
 * the count of active threads reaches 0, toggle the boolean
 * lock and notify waiting threads that the thread gang is idle/
 * ------------------------------------------------------------*/
int BkBarrier::Wait()
   {
   int status;
   bool  my_cycle;

   status = pthread_mutex_lock (&mutex);
   if (status != 0) err_abort(status, "pthread_mutex_lock");
   my_cycle = cycle;

   if (--counter == 0) 
      {
      counter = nTh;
      //cycle = !cycle;   // wrong! last thread will not wait
      is_idle = true;
      pthread_cond_broadcast(&qidle);  
      }
 
   while (my_cycle == cycle) 
       {
       status = pthread_cond_wait(&qtask, &mutex);
       if (status != 0) err_abort(status, "Cond Wait");
       }

   pthread_mutex_unlock (&mutex); 
   return status;          // error, or 0 
   }

void BkBarrier::WaitForIdle()
   {
   bool my_cycle;

   if(is_idle==true) return;
   pthread_mutex_lock (&mutex); 
   while(!is_idle)
       {
       int status = pthread_cond_wait(&qidle, &mutex);
       if (status != 0) err_abort(status, "Cond Wait");
       }
   pthread_mutex_unlock (&mutex); 
   }

void BkBarrier::ReleaseThreads()
   {
   pthread_mutex_lock (&mutex);
   is_idle = false;
   cycle = !cycle;
   pthread_cond_broadcast(&qtask);  
   pthread_mutex_unlock (&mutex); 
   }

void BkBarrier::WakeUpClient()
   {
   pthread_mutex_lock (&mutex);
   is_idle = true;
   cycle = !cycle;
   pthread_cond_signal(&qidle);  
   pthread_mutex_unlock (&mutex); 
   }



// *******************************************************

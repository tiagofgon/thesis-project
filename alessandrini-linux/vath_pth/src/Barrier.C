// **************************************
// Copyright (c) 2011 Victor Alessandrini
// All rights reserved.
// **************************************
/* -----------------------------------------
 * Barrier.C
 * This file implements the Barrier class.
 * ---------------------------------------*/

#include <pthread.h>
#include <Barrier.h>
#include <errors.h>


// Constructor
// -----------

Barrier::Barrier(int count)
   {
   int status;

   // Initialize fields and Pthreads structures 
   // -----------------------------------------
   
   nTh = counter = count;
   cycle = true;
   status = pthread_mutex_init (&mutex, NULL);
   if (status != 0) err_abort(status, "pthread_mutex_init");
   status = pthread_cond_init (&cond, NULL);
   if (status != 0) err_abort(status, "pthread_cond_init");
   }
 

// Destructor.
// ----------
   
Barrier::~Barrier ()
   {
   pthread_mutex_destroy (&mutex);
   pthread_cond_destroy (&cond);
   }

/* ------------------------------------------------------------
 * Wait for all members of a barrier to reach the barrier. When
 * the count of active threads reaches 0, broadcast to wake
 * all threads waiting.
 * ------------------------------------------------------------*/
   
int Barrier::Wait ()
   {
   int status, cancel, tmp;
   bool  my_cycle;

   pthread_mutex_lock(&mutex);
   my_cycle = cycle;

   if (--counter == 0) 
      {
      counter = nTh;
      cycle = !cycle;
      status = pthread_cond_broadcast (&cond);
	   
      // The last thread returns -1 rather than 0 
      if (status == 0) status = -1;
      } 
   else 
      {
      // Wait with cancellation disabled, because barrier_wait
      // should not be a cancellation point.
      // -----------------------------------------------------
       pthread_setcancelstate (PTHREAD_CANCEL_DISABLE, &cancel);
       
       while (my_cycle == cycle) 
          {
          status = pthread_cond_wait(&cond, &mutex);
          if (status != 0) err_abort(status, "Cond Wait");
          }
       pthread_setcancelstate (cancel, &tmp);
       }

   pthread_mutex_unlock (&mutex); 
   return status;        // 0 of OK with (-1) for waker, or positive if error 
   }

/* ----------------------------------------------------*/

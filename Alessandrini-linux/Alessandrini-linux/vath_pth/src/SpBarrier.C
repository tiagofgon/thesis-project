// **************************************
// Copyright (c) 2011 Victor Alessandrini
// All rights reserved.
// **************************************
/* -----------------------------------------
 * SBarrier.c
 * This file implements the SBarrier class,
 * a spin lock barrier
 * ---------------------------------------*/
#include <SpBarrier.h>

// Constructor
// -----------
SpBarrier::SpBarrier(int count)
   {
   // Initialize fields and Pthreads structures
   nTh = counter = count;
   cycle = true;
   }

// Destructor.
// ----------
SpBarrier::~SpBarrier () { }

/* ------------------------------------------------------------
 * Wait for all members of a barrier to reach the barrier. When
 * the count of active threads reaches 0, broadcast to wake
 * all threads waiting.
 * ------------------------------------------------------------*/
   
void SpBarrier::Wait ()
   {
   int cancel, tmp;
   bool  my_cycle, cycle_bis;

   lock.Lock();
   my_cycle = cycle;
   --counter;
   if (counter == 0) 
      {
      counter = nTh;
      cycle = !cycle;
      lock.Unlock();
      } 
   else 
      {
      lock.Unlock();
      do              // busy wait
         {
         lwait.Lock();
         cycle_bis = cycle;
         lwait.Unlock();
         }while(my_cycle==cycle_bis);
      }
   }
      
/* ----------------------------------------------------*/

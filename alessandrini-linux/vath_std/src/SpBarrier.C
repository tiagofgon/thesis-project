// **************************************
// Copyright (c) 2015 Victor Alessandrini
// All rights reserved.
// **************************************
/* -----------------------------------------
 * SBarrier.c
 * This file implements the SBarrier class,
 * a spin lock barrier
 * ---------------------------------------*/

#include <SpBarrier.h>

// Conxtructor
// -----------

SpBarrier::SpBarrier(int count)
   {
   nTh = counter = count;
   cycle = true;
   }

// Destructor.
// ----------
   
SpBarrier::~SpBarrier (){}

/* ------------------------------------------------------------
 * Wait for all members of a barrier to reach the barrier. When
 * the count of active threads reaches 0, broadcast to wake
 * all threads waiting.
 * ------------------------------------------------------------*/
   
void SpBarrier::Wait ()
   {
   bool  my_cycle, cycle_bis;

   lmutex.Lock();
   my_cycle = cycle;
   --counter;

   if (counter == 0) 
      {
      counter = nTh;
      cycle = !cycle;
      lmutex.Unlock();
      } 
   else 
      {
      lmutex.Unlock();
      do
         {
         lwait.Lock();
         cycle_bis = cycle;
         lwait.Unlock();
         }while(my_cycle==cycle_bis);
      }
   }
      
/* ----------------------------------------------------*/

// **************************************
// Copyright (c) 2011 Victor Alessandrini
// All rights reserved.
// **************************************
/* -----------------------------------------
 * Barrier.C
 * This file implements the Barrier class.
 * ---------------------------------------*/
#include <Barrier.h>

// Constructor
// -----------

Barrier::Barrier(int n)
   {
   Nth = n;
   count = n;
   predicate = true;
   }


// Destructor.
// ----------
Barrier::~Barrier () {}

int Barrier::Wait()
   {
   bool my_flag;
   int  retval = 0;

   // -----------------------------------------------------
   // Acquire mutex and decrease count. If count>0, wait on 
   // condition. If count==0, print ID and broadcast wake up.
   // ------------------------------------------------------
   std::unique_lock<std::mutex> lock(count_mutex);
   my_flag = predicate;
   count--;
   if(count)
      {
      while(my_flag==predicate) count_cond.wait(lock); 
      }
   else
      {
      predicate = !predicate;
      count = Nth;
      retval = -1;
      count_cond.notify_all();
      }
   return retval;
   }

/* ----------------------------------------------------*/

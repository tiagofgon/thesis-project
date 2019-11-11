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

// BlockBarrier.C
// This file implements the Barrier class.
// ---------------------------------------*/
#include <BkBarrier.h>

// Constructor
// -----------
BkBarrier::BkBarrier(int count)
   {
   nTh = counter = count;
   cycle = true;
   is_idle = false;
   }
 
// Destructor.
// ----------
BkBarrier::~BkBarrier () {}

/* ------------------------------------------------------------
 * Wait for all members of a barrier to reach the barrier. When
 * the count of active threads reaches 0, toggle the boolean
 * lock and notify waiting threads that the thread gang is idle/
 * ------------------------------------------------------------*/
int BkBarrier::Wait()
   {
   int retval = 0;
   bool  my_cycle;

   // Lock mutex for the rest of the function.
   // Mutex is automatically unlocked at exit
   // -----------------------------------------
   std::unique_lock<std::mutex> lock(bmutex);

   my_cycle = cycle;
   if (--counter == 0) 
      {
      counter = nTh;
      is_idle = true;
      qidle.notify_all(); 
      retval = -1; 
      }
 
   while (my_cycle == cycle) qtask.wait(lock);
   return retval;          // 0 or (-1) 
   }

void BkBarrier::WaitForIdle()
   {
   bool my_cycle;

   if(is_idle==true) return;

   // Lock mutex for the rest of the function.
   // Mutex is automatically unlocked at exit
   // -----------------------------------------
   std::unique_lock<std::mutex> lock(bmutex);
   while(!is_idle) qidle.wait(lock);
   }

void BkBarrier::ReleaseThreads()
   {
   // Lock mutex for the rest of the function.
   // Mutex is automatically unlocked at exit
   // -----------------------------------------
   std::unique_lock<std::mutex> lock(bmutex);
   is_idle = false;
   cycle = !cycle;
   qtask.notify_all();  
   }

// *******************************************************

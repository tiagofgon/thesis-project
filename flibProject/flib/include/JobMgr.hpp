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

// JobMgr.h
// --------
// This is an auxiliary class, part of the NPool 
// utility. It corresponds o the job manager that
// is created for each submitted job, monitoring 
// the job execution and synchronizing with the
// client threads
// **************************************
#ifndef JOBMGR_H
#define JOBMGR_H

#include <list>
#include <mutex>
#include "ThDeque.hpp"
#include "Task.hpp"
#include "BLock.hpp"

// ==================================
// Class JobCounter:  Auxiliary class
// ==================================
//
// The purpose of this class is to keep track of
// the number of jobs in execution in the pool, and 
// to signal to external client threads that the 
// pool is idle when there are no more ongoing
// jobs. This class implements the WaitforIdle
// utility.
//
// Internal Boolean lock state: 
//  - true if pool is inactive
//  - false if there are ongoing jobs
// ----------------------------------------------

class JobCounter
   {
   private: 
    int nJobs;
    bool flushFlag;
    std::mutex jcMutex;
    BLock *jcBLock;

   public:

     JobCounter()
        {
        nJobs = 0; 
        flushFlag = true; 
        jcBLock = new BLock(true); 	
        }
   
     ~JobCounter()
        {
        delete jcBLock; 
        }

     bool RegisterJob()
        {
        bool retval;
            {
            std::lock_guard<std::mutex> lock(jcMutex);
            retval = flushFlag;
            if (nJobs == 0)
               {
               jcBLock->SetState(false);
               flushFlag = false;
               }
            nJobs++;
            }
        return retval;
        }

     void UnregisterJob()
        {
        std::lock_guard<std::mutex> lock(jcMutex);
        nJobs--;
        if(nJobs==0)
           {
           flushFlag = true;           
           jcBLock->Set_And_Notify_All(true);
           }
        }

     BLock *GetIdleBLock() { return jcBLock; }

     bool PoolIsIdle()
        {
        bool retval;
            {
            std::lock_guard<std::mutex> lock(jcMutex);
            if (nJobs == 0) retval = true;
            else retval = false;
            }
        return retval;
        }

     int NJobs()
        {
        int retval;
           {
           std::lock_guard<std::mutex> lock(jcMutex);
           retval = nJobs;
           }
        return retval;
        }
     };


// ==============================
// Class JobMgr - Auxiliary class
// ==============================
//
// The purpose of the JobMgr class is to manage a group of tasks, and 
// in particular to run a boolean lock associated to the parallel job,
// that will remain in a "true" state as long as the whole group of
// tasks in the job is still active.
//
// The group of tasks is dynamic. This job manager takes into account
// the tests that are initially submitted to the pool, the tasks that
// are dynamically created (spawned), as well as the tasks that exit
// the group at termination. When there are no more tasks, the "end 
// of job" is  signalled to client threads.
//
// NOTICE: in this class, the internal mutex "gmutex" protects the 
// shared state variable "n_tasks", which is the number of tasks still
// active in the group. But the public Boolean Lock is not protected 
// by this mutex, because this object has internally its own mutex 
// that ensures its thread safety. 
//
// Boolean lock state: "false" means that the associated job is running,
// so that a WaitForJob call waits. "true" means that the target job is
// finished.

// Then, the MBLock moves to a wait (false) state when the job is 
// launched. When the job terminates, the notification is sent with
// a "true" argument, which wakes up threads and leaves the MBLock
// object in a "true" (no wait) state. In this way, threads do not
// wait if they call the wait function AFTER the notification.
//
// Mars 2014 modification: Job is registered and unregistered
// from the job counter diretcly by the associated job manager
// ________________________________________________________________

class JobMgr
   {
   private:
   ///////

   TaskGroup         *tgr;
   std::mutex        jmMutex;
   int               n_tasks;
   JobCounter        *JC;
   bool              cleanup_flag;
 

   public:
   //////
   BLock  gBlock;      // false by default
   bool   status;      // false=running, true=terminated
   
   JobMgr(JobCounter *jc, int nt=0)
      {
      JC = jc;
      n_tasks = nt;
      status = false;
      cleanup_flag = true;
      //gBlock.SetWait();           // mark "in use"
      }
   
   ~JobMgr()
      {
      if(cleanup_flag) CleanupJob();
      }

   void CleanupJob()
      {
      // Deallocate all the tasks in the group
      // -------------------------------------
      std::list<Task*>::iterator pos;
      for(pos=tgr->LT.begin(); pos!=tgr->LT.end(); pos++)
         {
         Task *t = *pos;      //  recover the Task* pointed by pos
         delete t; 
         }
      // Deallocate the task group itself
      // --------------------------------
      tgr->Clear();
      delete tgr;
      cleanup_flag = false;
      }

   void SetTaskGroup(TaskGroup *tg)
      {
      std::lock_guard<std::mutex> lock(jmMutex);
      tgr = tg; 
      }

   // -------------------------------------------
   // The next function is called by the worker
   // threads in a thread pool, after they finish
   // a task.
   // ------
   // NOTICE: if the mutex lock incorporates the 
   // notification, it is possible that a thread
   // waiting for the job wakes up and tries to 
   // destroy the JobMgr BEFORE the mutex is
   // unlocked. In this case, the destruction of
   // a locked mutex gives an error.
   // ------------------------------------------
   void DecreaseActive()
      {
      std::lock_guard<std::mutex> lock(jmMutex);
      n_tasks--;
      if(n_tasks==0)
         {
         status = true;
         JC->UnregisterJob();
         gBlock.Set_And_Notify_All(true);  // signal end of job       
         }
      }

   // ------------------------------------------
   // The next function is called by the worker
   // threads in a thread pool, when they post
   // a child task. The child task is added to 
   // the initial TaskGroup.
   // ------------------------------------------
   void IncreaseActive(Task *t)
      {
      std::lock_guard<std::mutex> lock(jmMutex);
      tgr->Attach(t);
      n_tasks++;
      }
   };

#endif

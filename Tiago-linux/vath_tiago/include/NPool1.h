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

// NPool.h 
// -------
// Master class for the NPool thread pool utility
// *********************************************
#ifndef NPOOL_H
#define NPOOL_H

#include "ThDeque.h"
#include "Task.h"
#include "ThreadMgr.h"
#include "JobMgr.h"
#include "SafeCounter.h"
#include <map>
#include <vector>
#include <array>
#include <atomic>

// =========================================================
// Class NPool1 implements very basic features:
//
// Launching and joining worker threads with the ThreadMgr
// class

// Testing the submission of successive lists of tasks, to
// validate the operation of the task queue. This version 
// must validate the most basic job submission and execution 
// mechanism adopted in this software.
//
// NO Job buffering
// NO Job monitoring
// NO spawned tasks
// NO taskwait
// NO wait for job
// NO wait for idle
//
// Class NPool2:
// ------------
//  * Adds intermedate buffering through a job queue, but
//  * withouth job by bob synvhronization yet.
//  *
//  * Modifies submission by introducing a copy of the 
//  * TaskGroup so that the external taskGroup can be
//  * reused.
//  -----------------------------------------------------
//  
//  Class NPool3
//  Implements Job buffering
//  Job buffering works
//
//  -----------------------------------------------------
//  
//  Class NPool4
//  Incorporates the JobCounter class, which implements
//  the WaitForIdle() utility called by client threads. 
//
// NO Job monitoring
// NO spawned tasks
// NO taskwait
// NO wait for job
//
// -------------------------------------------------------
//
// Class NPool5
// Incorporates Job monitoring
//
// No spawned tasks
// No taskwait
//
// ------------------------------------------------------
//
// Class NPool6
// Incorporates spawned tasks
//
// =========================================================

class NPool
   {
   private:

   // pool characteristics 
   // --------------------
   int      nThreads;          // number of threads in the pool
   int      nWorkers;          // number of active threads at a given time
   int      nTasks;
   int      last_key;          // tracks keys in the map container

   ThDeque<TaskGroup*>     *QJob;    // reference to internal job queue
   ThDeque<Task*>          **QTaskArray;   // Array of references to internal task queues
   ThreadMgr               *TM;      // managment of thread-task mapping
   //std::unique_ptr<ThreadMgr> TM;
   JobCounter              JC;       // counts running jobs
   //std::map<int, JobMgr*>  M;        // makagement of task groups
   std::map<int, std::shared_ptr<JobMgr>>  M;        // makagement of task groups


   // pool synchronization 
   // --------------------
   std::mutex              poolMutex;   // mutual exclusion on class data

   // For debugging purposes
   SafeCounter SC;
   
   bool  FlushJobQueue();
   void  TPool_Thread(int n);              // internal thread function
   void  DebugWait(const char* s);    // for debugging
   void  FlushTasks(TaskGroup *tg); 
   int   SuspendAndRunTask();

   public:

   NPool(int nThreads, int nMax=100);	
   ~NPool();
   int   GetThreadRank();

   int   SubmitJob(TaskGroup *tg);
   int   SubmitJob(Task *t);           
   void  WaitForIdle();               // waits for idle pool

   bool  JobStatus(int jid);
   void  WaitForJob(int jid);

   int   SpawnTask(Task *t, bool iswaited=true); 
   void  TaskWait();

   void  ClosePool();

   void  CheckMapping();   // debugging only

   friend void ThreadFunction(void *arg, int n);
   };

#endif

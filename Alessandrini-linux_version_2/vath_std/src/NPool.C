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
// NPool.C
// Master class for NPool utility
// =========================================================

#include <stdlib.h>
#include "NPool.h"
#include "Task.h"
#include "ThreadMgr.h"
#include <memory>

// ***************< THREAD FUNCTION >**********************
// This function is the thread function that will be passed 
// to pthreads_create. IT IS NOT a member function of the 
// NPool class. It calls the "true" thread function that 
// will be performed by the threads, and which is a member
// function of the NPool class (so that it can access its
// internal data items).
//
// This function receives as argument the address of the
// NPool object that implements the thread pool.
// -------------------------------------------------------

void ThreadFunction(void *arg)
   {
   NPool *NP;
   NP = (NPool *)arg;      // get reference to Thread Pool
   NP->TPool_Thread();     // call ThPool internal member function
   }

// ============
// Class NPool 
// ============


// -----------------------
// Constructor: NO CHANGE
// ----------------------
NPool::NPool(int nTh, int nMax)
   {
   // initialize the fields, copying the input parameters 
   // --------------------------------------------------

   nThreads = nTh;
   nWorkers = 0;
   nTasks = 0;
   last_key = 10;

   // Create task and Job queues
   // --------------------------
   //QTask = new ThDeque<Task*>();
   //QTask = std::make_unique<ThDeque<Task*>>();
   std::unique_ptr<ThDeque<Task*>> QTask;
   
   //QJob  = new ThDeque<TaskGroup*>();
   QJob  = std::make_unique<ThDeque<TaskGroup*>>();

   // Allocate ThreadManager
   // ----------------------
   //TM = new ThreadMgr(nThreads, this);
   TM = std::make_unique<ThreadMgr>(nThreads, this);

   }

// ----------------------
// Destructor : NO CHANGE
// ----------------------
NPool::~NPool() 
    {
    QTask->CloseQueue();

    // --------------------------------------------------
    // Notice: the order in which we call the destructors
    // is critical to avoid segmentation faults arising 
    // from the destruction of the queue mutexes while
    // being used by worker threads of the Q task queue.
    // Threads may be waiting for the "not empty" condition
    // and the Q->CloseQueue() call releases them. Then, the
    // TM destructor joins them. Fine. 
    // If the Q destructor is called first, the queue may be
    // destroyed while idle threads are still waiting to be
    // released.
    // ---------------------------------------------------
    //delete TM;
    //delete QTask;
    //delete QJob;
    }

// ----------------------------------------------
// FlushJobQueue()
// THIS FUNCTION IS CALLED WITH LOCKED POOL MUTEX
// ----------------------------------------------
bool NPool::FlushJobQueue()
   {
   bool state;
   TaskGroup *tg = QJob->TryRemoveFront(state);
   if(state) FlushTasks(tg);
   return state;
   }

void NPool::FlushTasks(TaskGroup *tg)
   {
   int rank = TM->GetRank();	     
   std::list<Task*>::iterator pos;
   for(pos=tg->LT.begin(); pos!=tg->LT.end(); pos++)
      {
      Task *t = *pos;      //  recover the Task* pointed by pos
      QTask->Add(t);
      nTasks++;
      }   
   }

// -------------------------------------------------
// Submit a sequential job composed of a single task
// There are no basic changes in this function, besides
// the initialization of a number of data items
// -------------------------------------------------
int NPool::SubmitJob(Task *T)
   {
   bool flushflag;
   int rank = TM->GetRank();

   if(rank>0) return 0;      // client threads have rank (-1);
   else
      {
      JobMgr *JM = new JobMgr(&JC, 1);     
          {     // enter critical section
          std::lock_guard<std::mutex> lock(poolMutex);
          last_key++;                         
          M.insert(make_pair(last_key, JM)); 
          T->SetJobid(last_key);            
	  T->SetTaskID( SC.Next() );   // next SafeCounter integer
          TaskGroup *tg = new TaskGroup();
          tg->Attach(T);
	  JM->SetTaskGroup(tg);     // NEW!!
          QJob->Add(tg);
          flushflag = JC.RegisterJob();
          if( flushflag ) FlushJobQueue();
          }
      return last_key;
      }
   }

// ----------------------------------------------
// Submit a job composed of a group of tasks
// There are no basic changes in this function,
// besides the initialization of a number of 
// data items
// ----------------------------------------------
int NPool::SubmitJob(TaskGroup *TG)
   {
   bool flushflag;
   int rank = TM->GetRank(); 
   if(rank>0) return 0;   // client threads have rank (-1)
   else
      {
      int sz = TG->LT.size();          
      JobMgr *JM = new JobMgr(&JC, sz); 
      
          {     // enter critical section
          std::lock_guard<std::mutex> lock(poolMutex);
          last_key++;                        
          M.insert(make_pair(last_key, JM)); 

          // ---------------------------------------------
          // We transfer the Task* of the submitted TG to
          // the new one, eventually adding information to
          // the tasks
          // --------------------------------------------
          TaskGroup *tg = new TaskGroup();
          std::list<Task*>::iterator pos;
          for(pos=TG->LT.begin(); pos!=TG->LT.end(); pos++)
             {
             Task *t = *pos;     
             t->SetJobid(last_key);     
	     t->SetTaskID( SC.Next() );  // next SafeCounter integer
             tg->Attach(t);
             }
          // ---------------------------------
          // Now, we insert the new task group
          // container in the job queue
          // ---------------------------------
	  JM->SetTaskGroup(tg);          // NEW!!
          QJob->Add(tg);
          flushflag = JC.RegisterJob();
          if(flushflag) FlushJobQueue();
          }
      return last_key;
      }
   }

// ----------------------------------------------------------
// SpawnTask() :
// ------------- 
// Creates a new explicit synchronized task inside a parallel 
// job. No new task group is created: the child task enters 
// the same task group as parent task.
//
// Tasks launched this way just integrate the ongoing job, and
// their termination is reported with the job termination report.
// But they can also be tracked individually: their termination
// is automatically notified by the system and the parent task
// cdcan synchronize with a child task termination by calling
// WaitForTask().
//
// Return value:
// ------------
// O if no action taken because queue is closed
// 1 if success
// ----------------------------------------------------------  
int NPool::SpawnTask(Task *tg, bool iswaited)
   {
   int key;
   if(iswaited==false) tg->SetWaited(false);
   
      {
      std::lock_guard<std::mutex> lock(poolMutex);
      // Get the key of the current thread group to
      // which the executing thread belongs. This
      // function is called by worker threads
      // ------------------------------------------
      Task *Ctask = TM->GetCurrTask();
      key = Ctask->GetJobid();
      tg->SetJobid(key);
	  tg->SetTaskID( SC.Next() );
   
      if(tg->Am_I_Waited()) tg->SetParent(Ctask);
      else tg->SetParent(NULL);
      // -----------------------------------------
      // This executing task launches a child task.
      // Increase its reference count
      // -----------------------------------------
      Ctask->IncreaseRefcount();

      // ---------------------------------------------
      // Inform JobMgr that a new task is incorporated
      // into the job
      // ---------------------------------------------
      M[key]->IncreaseActive(tg);

      // Post directly to task queue
      // ---------------------------
      QTask->Add(tg);
      nTasks++;
      }
   return 1; 
   }

// ------------------------------------
// ClosePool()
// ---------- 
// Stops worker threads, by closing the
// task queue
// ------------------------------------   
void NPool::ClosePool()
   {
   QTask->CloseQueue();
   }

// --------------------------------
// Returns ID rank of caller thread
// --------------------------------
int NPool::GetThreadRank()
   {
   int rank = TM->GetRank();
   return rank;
   }

// ------------------------------------------------------
// WaitForIdle()
// -------------
// Called by the master thread that is running the pool, 
// to wait for an "idle" state, making sure that all the 
// work requests have been serviced.
// -------------------------------------------------------
void NPool::WaitForIdle()
   {
   BLock *bl = JC.GetIdleBLock();
   bl->Wait_Until(true, 0L);
   }
 
// ----------------------------------------
// TPool_thread()
// --------------
// This is the startup routine executed by 
// all worker threads.
// ---------------------------------------   
void NPool::TPool_Thread()
   {
   int rank, key;
   bool state;
   Task* T, *my_parent;

   for(;;) // Here we start an infinite loop
      {
      T = QTask->Remove(state);      // read task address 
      if(state==false) break;

      // -------------------------------------------
      // Set up the data items needed to manage the
      // mapping of tasks to threads
      // -------------------------------------------
      my_parent = T->GetParent();
      rank = TM->GetRank();
      T->SetOwnerRank(rank);
      TM->SetCurrTask(T);

      // ------------------------------
     // Get the JobMgr in charge of this task
      // ------------------------------------
      key = T->GetJobid();
      JobMgr *JM = M[key];
      
      // Increase nWorkers
      // -----------------
          {
          std::lock_guard<std::mutex> lock(poolMutex);
          nWorkers++;
          }

      // Execute the task function
      // -------------------------
      T->ExecuteTask();

      // Decrease refcount of parent
      // ---------------------------
      if(my_parent!=NULL && T->Am_I_Waited()) 
            my_parent->DecreaseRefcount(); 
      
      // Decrease nWorkers and eventually
      // unregister and flush queue
      // -------------------------------
          {
          std::lock_guard<std::mutex> lock(poolMutex);
          nWorkers--;
          JM->DecreaseActive();     // job is unregistered here
          if(nWorkers==0) FlushJobQueue();
          }
      }  // end for           
   }

// ----------------------------------------------------
// JobStatus()
// -----------
// This function returns the job status:
// true   : running
// false  : terminated
//
// This function is executed by a client thread external
// to the pool, or by a worker thread in the pool not
// involved in the job (in the case of nested jobs)
// ----------------------------------------------------

bool NPool::JobStatus(int key)
   {
   bool retval;
   JobMgr *JM = M[key];
   retval = JM->status;
   return retval;
   } 

// ----------------------------------------------------
// WaitForJob()
// ------------
// This function waits for individual job termination. 
// Then, it removes the job manager from the map, and 
// deletes the JobMgr object. In this way, the reference
// to the terminated job dissappears from the system.
//
// This function is executed by a client thread external
// to the pool, or by a worker thread in the pool not
// involved in the job (in the case of nested jobs)
// ----------------------------------------------------
void NPool::WaitForJob(int key)
   {
   if(key==0)
      {
      std::cout << "\n Incorrect call to WaitForJob: job ID is 0"
                << std::endl;
      exit(0);
      }
   else
      {
      JobMgr *JM = M[key];
      JM->gBlock.Wait_Until(true, 0L);
      M.erase(key);   // JM pointer is removed, but not deleted
      //delete JM;
      }
   } 

// ------------------------------------
// SuspendAndRunTask()
// ----------------------
// Called by a worker thread before a 
// TaskWait is engaged
// ------------------------------------
int NPool::SuspendAndRunTask()
   {
   Task *T, *my_parent, *save_task;
   int key;
   bool state;
   JobMgr *JM;
	
   T = QTask->TryRemoveBack(state);      // read task address
   if(state==false) return 0;

   // Here, we have removed a task for execution
   // ------------------------------------------
   my_parent = T->GetParent();
   int rank = TM->GetRank();
   T->SetOwnerRank(rank);

   // In the ThreadManager, we have to switch the tasks that
   // are run by this thread
   // ------------------------------------------------------
   save_task = TM->GetCurrTask();
   TM->SetCurrTask(T);

   key = T->GetJobid();        // identify job manager  
   if(key>0) JM = M[key];
   
   // Execute the task function, and eventually 
   // signal for taskwait
   // ----------------------------------------
   T->ExecuteTask();
   if(my_parent!=NULL && T->Am_I_Waited()) 
         my_parent->DecreaseRefcount(); 

   // -----------------------------------------------
   // Now, decrease n_active in the Tgroup. If we
   // reach 0, the task group will signal termination..
   // -----------------------------------------------
   if(key>0) JM->DecreaseActive();  // signaling here

   // restore the pointer to the suspended task
   // -----------------------------------------
   TM->SetCurrTask(save_task);
   return 1;
   }

// ----------------------------------------------------
// TaskWait();
// -----------
// This function is called by a worker thread, to wait 
// for the childs of the running task. Before waiting, 
// the thread suspends the execution of the current 
// (ready to wait) task for as long ans there are 
// unserviced tasks in the task queue. In this way, 
// we are sure that children are executed, and the 
// system does not deadlock
// ----------------------------------------------------
void NPool::TaskWait()
   {
	int retval;
   do
      {
      //std::cout << "\n Calling Suspend" << std::endl;
      retval = SuspendAndRunTask();
      //std::cout << "\n Suspend returned " << retval << std::endl;
      } while(retval);
   Task *tk = TM->GetCurrTask();
   //std::cout << "\n Task " << tk->GetTaskID() <<" waiting for childs" << std::endl;
   tk->WaitForChilds();
   //std::cout << "\n Taskwait: returning from wait" << std::endl;
   } 

void NPool::CheckMapping()
    {
    int n;
	std::lock_guard<std::mutex> lock(poolMutex);
	for (n = 1; n <= nThreads; ++n)
	    {
		Task *t = TM->curr_task[n];
		std::cout << "\n Rank " << n << " thread running task "
			<< t->GetTaskID();
	    }
	std::cout << std::endl;
    }
//******************************************************

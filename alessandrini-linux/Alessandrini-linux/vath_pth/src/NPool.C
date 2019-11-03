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
// This is version 6 code. 
// LAST VERSION
//
// Changes: new versions of SubmitJob() and WaitForJob()
// 
//----------
#include <stdlib.h>
#include <pthread.h>
#include <NPool.h>
#include <errors.h>
#include <Common.h>

#define BUFFERING
#define SUSPENDED_TASKS

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

void *ThreadFunction(void *arg)
   {
   NPool *NP;
   NP = (NPool *)arg;      // get reference to Thread Pool
   NP->TPool_Thread();     // call ThPool internal member function
   return NULL;
   }

// ****************
// Class NPool 
// ****************

NPool::NPool(int nTh, int nMax)
   {
   int n, status;

   // initialize the fields, copying the input parameters 
   // --------------------------------------------------

   nThreads = nTh;
   maxQsize = nMax;
   nWorkers = 0;
   last_key  = 10;

   // Create task queue
   // -----------------
   Q = new ThDeque<Task*>();
   QJ = new ThDeque<TaskGroup*>();

   // initialize mutex
   // -----------------
   status = pthread_mutex_init( &qLock, NULL);
   if(status != 0) err_abort(status, "Mutex Init");

   // Allocate ThreadManager
   // ----------------------
   TM = new ThreadManager(nThreads, this);
   }


// Destructor
// -----------
NPool::~NPool() 
    {
    WaitForIdle();
    Q->CloseQueue();
    pthread_mutex_destroy(&qLock);

    // --------------------------------------------------
    // Notice: the order in which we call the destructors
    // is critical to avoid segmentation faults arising 
    // from the destruction of the queue mutexes while
    // being used by worker threads of the Q task queue.
    // Threads my be waiting for the "not empty" condition
    // and the Q->CloseQueue() call releases them. Then, the
    // TM destructor joins them. Fine. 
    // If the Q destructor is called first, the queue may be
    // destroyed while idle threads are still waiting to be
    // released.
    // ---------------------------------------------------
    delete TM;
    delete Q;
    delete QJ;
    }

bool NPool::FlushJobQueue()
   {
   // THIS FUNCTION IS CALLED WITH LOCKED qLock MUTEX
   // -----------------------------------------------
   bool state;
   TaskGroup *tg = QJ->TryRemoveFront(state);
   if(state)
      {
      std::list<Task*>::iterator pos;
      for(pos=tg->LT.begin(); pos!=tg->LT.end(); pos++)
         {
         Task *t = *pos;      //  recover the Task* pointed by pos
         Q->Add(t); 
         }
      }   
   return state;
   }


void NPool::FlushTaskGroup(TaskGroup *tg)
   {
   // THIS FUNCTION IS CALLED WITH LOCKED qLock MUTEX
   // -----------------------------------------------
   std::list<Task*>::iterator pos;
   for(pos=tg->LT.begin(); pos!=tg->LT.end(); pos++)
      {
      Task *t = *pos;      //  recover the Task* pointed by pos
      Q->Add(t); 
      }
   }

// ----------------------------------------------------------
// Two SubmitJob() functions:
// Return an intager which is the Job ID, needed to query for 
// job status or wait for job termination.
// ----------------------------------------------------------  

// Submit a sequential job composed of a single task
// *************************************************
int NPool::SubmitJob(Task *T)
   {
   int rank = TM->GetRank();
   if(rank) return 0;
   else
      {
      // Job is submitted by an external client thread
      // Create group of size 1
      // -----------------------
      JobMgr *TG = new JobMgr(&JC, 1);
      
      Pthread_Mutex_LockBis(&qLock, "SubmitJob(), 1");
      // _____________________________________
      last_key++;
      M.insert(make_pair(last_key, TG));
      T->SetJobid(last_key);
      TaskGroup *tg = new TaskGroup();
      tg->Attach(T);
      TG->SetTaskGroup(tg);
      JC.RegisterJob();

      QJ->Add(tg);
      if(nWorkers==0) FlushJobQueue();
      // ______________________________________
      Pthread_Mutex_Unlock(&qLock);
      return last_key;
      }
   }

// Submit a job composed of a group of tasks
// *****************************************
int NPool::SubmitJob(TaskGroup *tg)
   {
   int rank = TM->GetRank(); 
   if(rank) return 0;
   else
      {
      // First, construct the job manager
      // --------------------------------
      int size = tg->LT.size();
      JobMgr *TG = new JobMgr(&JC, size);
      
      Pthread_Mutex_LockBis(&qLock, "SubmitJob(), 2");   // lock pool mutex
      // ..............................................  
      last_key++;
      M.insert(make_pair(last_key, TG));
  
      // -----------------------------------------------------
      // We need to add information to the tasks encapsulated
      // in the task group. Each task must know the ID of the
      // job it belongs. We move the tasks from the passed task
      // group to a new one (std::list cannot be modified "in 
      // place"). The job manager knows about the modified task
      // group.
      // ---------------------------------------------------- 
      std::list<Task*>::iterator pos;
      TaskGroup *tgroup = new TaskGroup();
      for(pos=tg->LT.begin(); pos!=tg->LT.end(); pos++)
         {
         Task *t = *pos;     
         t->SetJobid(last_key);
         tgroup->Attach(t);
         }
      TG->SetTaskGroup(tgroup);

      QJ->Add(tgroup);
      if(nWorkers==0) FlushJobQueue();
      JC.RegisterJob();
      // .....................................................
      Pthread_Mutex_Unlock(&qLock);        // unlock pool mutex
      return last_key;
      }
   }

// ----------------------------------------------------------
// SpawnTask() : creates a new explicit synchronized task 
// inside a parallel job. No new task group is created: the 
// child task enters the same task group as parent task.
//
// Tasks launched this way just integrate the ongoing job, and
// their termination is reported with the job termination report.
// But they can also be tracked individually: their termination
// is automatically notified by the system and the parent task
// can synchronize with a child task termination by calling
// WaitForTask().
//
// Return value:
// ------------
// O if no action taken because queue is closed
// 1 if success
// ----------------------------------------------------------  
/*
int NPool::SpawnTask(Task *tg, bool iswaited)
   {
   int key, bkey, rank;
   if(iswaited==false) tg->SetWaited(false);
   
   Pthread_Mutex_LockBis(&qLock, "SpawnTask()");
   // ------------------------------------------
   rank = SC.Next();
   // std::cout << "\n Inside spawn: " << rank << std::endl;
   // ---------------------------------------
   // Get the key of the current thread group
   // ---------------------------------------
   Task *Ctask = TM->GetCurrTask();
   key = Ctask->GetJobid();
   tg->SetJobid(key);

   if(tg->Am_I_Waited()) tg->SetParent(Ctask);
   else tg->SetParent(NULL);

   Ctask->IncreaseRefcount();
   // Pthread_Mutex_LockBis(&qLock, "SpawnTask()");
   // ------------------------
   // std::cout << "\n Accesssing NPool object: " << rank << std::endl;
   M[key]->IncreaseActive(tg);
   Q->Add(tg);
   //std::cout << "\n Exit spawn: " << rank << std::endl;
   // ------------------------
   Pthread_Mutex_Unlock(&qLock);
   return 1;
   }
*/

int NPool::SpawnTask(Task *tg, bool iswaited)
   {
   int key, bkey, rank;
   if(iswaited==false) tg->SetWaited(false);
   
   Task *Ctask = TM->GetCurrTask();
   key = Ctask->GetJobid();
   tg->SetJobid(key);

   if(tg->Am_I_Waited()) 
      {
      tg->SetParent(Ctask);
      Ctask->IncreaseRefcount();
      }
   else tg->SetParent(NULL);

   Pthread_Mutex_LockBis(&qLock, "SpawnTask()");
   // ------------------------
   M[key]->IncreaseActive(tg);
   Q->Add(tg);
   // ------------------------
   Pthread_Mutex_Unlock(&qLock);
   return 1;
   }

// --------------------------------
// ClosePool() stops worker threads
// --------------------------------   
void NPool::ClosePool()
   {
   int n, retval;
   Q->CloseQueue();
   }

// --------------------------------------------------------------
// Next routine, TPool_thread(), is the startup routine executed
// by all worker threads.
// --------------------------------------------------------------
   
void NPool::TPool_Thread()
   {
   int retval;
   Task *T, *my_parent;
   int key;
   bool state;
   JobMgr *TG;
	
   for(;;) // Here we start an infinite loop
      {
      T = Q->Remove(state);      // read task address
      if(state==false) pthread_exit(NULL);

      my_parent = T->GetParent("GetParent, thread pool");
      int rank = TM->GetRank();
      T->SetOwnerRank(rank);
      TM->SetCurrTask(T);
      // ------------------------------

      key = T->GetJobid();        // identify job manager  
      TG = M[key];
      
      // increase nWorkers.
      // ---------------------------
      Pthread_Mutex_LockBis( &qLock, "Thread Function 1");
      nWorkers++;
      Pthread_Mutex_Unlock( &qLock );

      // Execute the task function, and eventually 
      // signal for taskwait
      // ----------------------------------------
      T->ExecuteTask();
     
      // Decrease refcount of parent
      // ---------------------------
      if(my_parent!=NULL && T->Am_I_Waited()) 
            my_parent->DecreaseRefcount(); 

      // ------------------------------------------
      // Task function has returned. Decrease nWorkers.
      // --------------------------------------------
      Pthread_Mutex_LockBis( &qLock, "Thread Function 2");
      nWorkers--;
      if(nWorkers==0) FlushJobQueue();
      Pthread_Mutex_Unlock( &qLock );

      // -----------------------------------------------
      // Now, decrease n_active in the JobMgr. If we reach 
      // 0, the JobMgr will signal job termination.
      // -----------------------------------------------
      if(key>0) TG->DecreaseActive();  // signaling here
      }  // end for           
   }

// --------------------------------------------------------
// Next routine, Suspend_And_Run_Task(), is called before a 
// TaskWait is engaged
// --------------------------------------------------------
   
int NPool::Suspend_And_Run_Task()
   {
   int retval;
   Task *T, *my_parent, *save_task;
   int key;
   bool state;
   JobMgr *TG;
	
   T = Q->TryRemoveBack(state);      // read task address
   if(state==false) return 0;

   my_parent = T->GetParent("GetParent, suspend and run");
   int rank = TM->GetRank();
   T->SetOwnerRank(rank);
   save_task = TM->GetCurrTask();
   TM->SetCurrTask(T);
   // ------------------------------

   key = T->GetJobid();        // identify job manager  
   if(key>0) TG = M[key];
   
   // Execute the task function, and eventually 
   // signal for taskwait
   // ----------------------------------------
   T->ExecuteTask();
   if(my_parent!=NULL && T->Am_I_Waited()) 
         my_parent->DecreaseRefcount(); 

   // -----------------------------------------------
   // Now, decrease n_active in the Tgroup. If we
   // reach 0, the task group will signal termination.
   // Finally, deallocate the task just executed.
   // -----------------------------------------------
   if(key>0) TG->DecreaseActive();  // signaling here

   // restore the pointer to the suspended task
   TM->SetCurrTask(save_task);
   return 1;
   }

// ------------------------------------------------------
// WaitForIdle() will be called by the master thread that 
// is running the pool, to wait for an "idle" state making 
// sure that all the work requests have been serviced.
// -------------------------------------------------------
void NPool::WaitForIdle()
   {
   BLock *bl = JC.GetIdleBLock();
   bl->Wait_Until(true, 0);
   }
 
// ----------------------------------------------------
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
      JobMgr *TG = M[key];
      TG->gBlock.Wait_Until(false, 0);
      M.erase(key);   // TG pointer is removed, but not deleted
      delete TG;
      }
   } 

// ------------------------------------------------------
// This function is called by a worker thread, to wait for
// the childs of the running task. Before waiting, the
// thread suspends the execution of the current (ready to
// wait) task for as long ans there are unserviced tasks
// in the task queue. In this way, we are ure that children
// are executed, and the system does not deadlock
// -------------------------------------------------------
void NPool::TaskWait()
   {
   int retval;
   #ifdef SUSPENDED_TASKS
   do
      {
      retval = Suspend_And_Run_Task();
      } while(retval);
   #endif
   Task *T = TM->GetCurrTask();
   T->WaitForChilds();
   } 

// ----------------------------------------------------
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
   bool status;
   JobMgr *TG = M[key];
   status = TG->gBlock.GetState();
   return status;
   } 

//******************************************************

/* ---------------------------------------------------------------------------
                           Made by Tiago Gonçalves - 2019
 --------------------------------------------------------------------------*/

// NPool.cpp
// Master class for NPool utility
// =========================================================

#include <stdlib.h>
#include "NPool.hpp"
#include "Task.hpp"
#include "ThreadMgr.hpp"
#include <iostream>

// ***************< THREAD FUNCTION >**********************
// This function is the thread function that will be passed 
// to pthreads_create. IT IS NOT a member function of the 
// NPool class. It calls the "true" thread function that 
// will be performed by the threads, and which is a member
// function of the NPool class (so that it can access its
// internal data items).
//
// This function receives as argument the address of the
// NPool object that implements the thread pool and an integer that
// represents the id of the owner thread on the pool.
// -------------------------------------------------------

void ThreadFunction(void *arg, int n)
   {
      
   NPool *NP;
   NP = (NPool *)arg;      // get reference to Thread Pool
   NP->TPool_Thread(n);     // call ThPool internal member function
   }

// ============
// Class NPool 
// ============


// -----------------------
// Constructor, new features:
// - Deque of tasks for each thread, not only one deque for all threads
// - ThreadManager as smart pointer (unique_pointer)
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
   QJob  = new ThDeque<TaskGroup*>();

   QTaskArray = new ThDequeThread<Task*> *[nThreads+1];
   for(int n=1; n<=nThreads; n++) {
      QTaskArray[n] = new ThDequeThread<Task*>();
   }
   
   // Allocate ThreadManager with make_unique
   TM = std::make_unique<ThreadMgr>(nThreads, this);

   }

// ----------------------
// Destructor
// - Delete the threadManager smart pointer with reset
// ----------------------
NPool::~NPool() 
    {
   for(int n=1; n<=nThreads; n++){
      QTaskArray[n]->CloseQueue();
   }

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

    //delete TM first;
    TM.reset();

    for(int n=1; n<=nThreads; n++){
      delete QTaskArray[n];
   }

    delete QJob;
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
   int i;
   for(pos=tg->LT.begin(), i=0; pos!=tg->LT.end(); pos++, i++)
      {
      Task *t = *pos;      //  recover the Task* pointed by pos

      // distribute the tasks by threads deques in round robin way
      QTaskArray[(i%nThreads)+1]->Add(t);
      nTasks++;
      }   
   }

// -------------------------------------------------
// Submit a sequential job composed of a single task
// -------------------------------------------------
int NPool::SubmitJob(Task *T)
   {
   bool flushflag;
   int rank = TM->GetRank();
   
   if(rank>0) return 0;      // client threads have rank (-1);
   else { 
      std::shared_ptr<JobMgr> JM = std::make_shared<JobMgr>(&JC, 1); 
      
          {     // enter critical section
          std::lock_guard<std::mutex> lock(poolMutex);
          last_key++;                         
          //M.insert(make_pair(last_key, JM)); 
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
// ----------------------------------------------
int NPool::SubmitJob(TaskGroup *TG)
   {
   bool flushflag;
   int rank = TM->GetRank(); 
   if(rank>0) return 0;   // client threads have rank (-1)
   else
      {
      int sz = TG->LT.size();          
      std::shared_ptr<JobMgr> JM = std::make_shared<JobMgr>(&JC, sz); 
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
      int rank = TM->GetRank();
      //std::cout << "rank= " << rank << std::endl;
      QTaskArray[rank]->Add(tg);
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
   for(int n=1; n<=nThreads; n++) {
      QTaskArray[n]->CloseQueue();
   }
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
void NPool::TPool_Thread(int n)
   {
   int rank, key;
   bool state, flag=false;
   Task* T, *my_parent;
   rank=n;

   for(;;) // Here we start an infinite loop
      {

      bool estado=true;
      while(estado){
         if(QTaskArray[rank]->GetSize()==0){
            for(int i=1; i<= nThreads; i++) {
               T = QTaskArray[i]->ThDequeThread::TryRemoveBack(flag);               
               if(flag==true) {
                  QTaskArray[rank]->Add(T);
                  break;
               }
            }
            estado = QTaskArray[rank]->getState();
            //std::cout << "estado: " << estado << std::endl;
         }
         else{
            estado=false;
         }
      }
      
      T = QTaskArray[rank]->Remove(state);      // read task address 
  
      if(state==false) break;
      //std::cout << "rank= " << rank << std::endl;

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
      std::shared_ptr<JobMgr> JM = M[key];
      
      // Increase nWorkers
      // -----------------
          {
          std::lock_guard<std::mutex> lock(poolMutex);
          nWorkers++;
          }

      // Execute the task function
      // -------------------------
      std::function<void(void)> taskFunction;
      taskFunction = std::move(T->taskFunction);
      taskFunction();

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
   std::shared_ptr<JobMgr> JM = M[key];
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
      std::shared_ptr<JobMgr> JM = M[key];
      JM->gBlock.Wait_Until(true, 0L);
      M.erase(key);   // JM pointer is removed, but not deleted
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
   std::shared_ptr<JobMgr> JM;
	int rank = TM->GetRank();   
   T = QTaskArray[rank]->TryRemoveBack(state);     // read task address
   if(state==false) return 0;

   // Here, we have removed a task for execution
   // ------------------------------------------
   my_parent = T->GetParent();
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
   std::function<void(void)> taskFunction;
   taskFunction = std::move(T->taskFunction);
   taskFunction();
   
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

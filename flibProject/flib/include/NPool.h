/* ---------------------------------------------------------------------------

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
   //ThDeque<Task*>          *QTask;   // reference to internal task queue
   ThDeque<Task*>          **QTaskArray;   // Array of references to internal task queues
   ThreadMgr               *TM;      // managment of thread-task mapping
   JobCounter              JC;       // counts running jobs
   //std::map<int, JobMgr*>  M;        // makagement of task groups
   std::map<int, std::shared_ptr<JobMgr>>  M;        // makagement of task groups

   // pool synchronization 
   // --------------------
   std::mutex              poolMutex;   // mutual exclusion on class data

   // For debugging purposes
   SafeCounter SC;
   
   bool  FlushJobQueue();
   void  TPool_Thread(int);              // internal thread function
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

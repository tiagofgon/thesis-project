/* ---------------------------------------------------------------------------
                           Made by Tiago Gonçalves - 2019
 --------------------------------------------------------------------------*/

// TaskCentricPool.hpp
// -------
// Master class for the TaskCentricPool thread pool utility
// *********************************************
#ifndef TaskCentricPool_H
#define TaskCentricPool_H

#include "ThDeque.hpp"
#include "ThDequeThread.hpp"
#include "Task.hpp"
#include "ThreadMgr.hpp"
#include "JobMgr.hpp"
#include "SafeCounter.hpp"
#include <map>
#include <vector>

// =========================================================
// Class TaskCentricPool implements new features:
//
// - Smart Pointers
// - Deque of tasks for each thread, not only one deque for all threads
// =========================================================

class TaskCentricPool
   {
   private:

   // pool characteristics 
   // --------------------
   int      nThreads;          // number of threads in the pool
   int      nWorkers;          // number of active threads at a given time
   int      nTasks;
   int      last_key;          // tracks keys in the map container

   ThDeque<TaskGroup*>     *QJob;    // reference to internal job queue
   ThDequeThread<Task*>          **QTaskArray;   // Array of references to internal task queues
   std::unique_ptr<ThreadMgr> TM;
   JobCounter              JC;       // counts running jobs
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

   TaskCentricPool(int nThreads, int nMax=100);	
   ~TaskCentricPool();
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

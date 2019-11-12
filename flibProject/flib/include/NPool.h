/* ---------------------------------------------------------------------------
    Made by Tiago Gonçalves
 --------------------------------------------------------------------------*/


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
   JobCounter              JC;       // counts running jobs
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

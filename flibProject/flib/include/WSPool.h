// **************************************
// Copyright (c) 2016 Victor Alessandrini
// All rights reserved.
// **************************************
// WSPool.h
// A thread pool class implementing work stealing
// ------------------------------------------
#include <iostream>
#include <BkBarrier.h>
#include <Task.h>
#include <Timer.h>
#include <WSDeque.h>
#include <RandInt.h>
#include <thread>
#include <vector>
#include <mutex>

void threadFunc(void *P, int rank);

// -------------------------------------------------------
// This is an auxiliary function, used to store the thread
// rank in a thread_local variable, and to retrieve it later
// on in other function calls:
//
// TLInteger(value, false) stores value, return value is 
// ignored
// 
// int rank = TLInteger(0, true) retrieves the stored value,
// the 0 argument is ignored
// -------------------------------------------------------- 
int TLInteger(int value, bool b)
   {
   thread_local int my_rank = 0;
   int retval = -1;

   if(b==false) my_rank = value;    // initialize
   else retval = my_rank;
   return retval;
   }

// --------------------------------------------------------------
// The purpose of this utility is to allow threads in a pool to
// know, by a call to ActiveTasks(), if all of them are inactive.
// -------------------------------------------------------------- 
class JobMgr
   {
   private:
   ///////
   std::atomic<int> task_count;

   public:
   ///////
    JobMgr()
       { task_count.store(0);}

    void RegisterTask()
       {
       task_count.fetch_add(1);
       }

    void UnregisterTask()
       {
       task_count.fetch_sub(1);
       }

    int ActiveTasks()
       { 
       int retval = task_count.load();
       return retval; 
       }
    };
      

class WSPool
   {
   private:
   ////////
    Timer T;
    BkBarrier *BB;               
    RandInt   *RI;
    WSDeque   *ws_queue;             // array of work stealing queues
    JobMgr JM;                       // job manager, tracks tasks
    std::vector<std::thread> workers;
    std::mutex pMutex;
    int       nTh;
    bool      shut;

    public:
    //////
    
    // Constructors and destructors
    // ----------------------------
    WSPool(int nThreads)
       {
       nTh = nThreads;
       BB = new BkBarrier(nTh);
       RI = new RandInt(nTh-1);
       ws_queue = new WSDeque[nTh];
       shut = false;

       for (int n=0; n<nTh; ++n)
          {
          workers.push_back(std::thread(threadFunc, (void*)this, n) );
          }
       T.Wait(500);
       }

    ~WSPool()
       {
       delete BB;
       delete RI;
       delete [] ws_queue;
       }

   // ---------------------------------------------------
   // Job submissions, called by client threads. Task and
   // TaskGroup classes are the same ones than in NPool
   // ---------------------------------------------------

   // Submit a job with a unique root task:
   // ------------------------------------
   void SubmitJob(Task *T)
       {
       ws_queue[0].push(T);
       JM.RegisterTask();
       BB->ReleaseThreads();
       }

   // Submit a job with several root tasks, encapsulated in a
   // TaskGroup container
   // -------------------------------------------------------
   void SubmitJob(TaskGroup *TG)
       {
       int n = 0;
       std::list<Task*>::iterator pos;
       for(pos=TG->LT.begin(); pos!=TG->LT.end(); pos++)
          {
          Task *T = *pos;
          int index = n%nTh; 
          ws_queue[index].push(T);    
          JM.RegisterTask();
          n++;
          }
       BB->ReleaseThreads();
       }

   // Launch a child task. Called by worker threads
   // ---------------------------------------------
   void SubmitChild(Task *T)
       {
       int myrank = TLInteger(0, true);
       ws_queue[myrank].push(T);
       JM.RegisterTask();
       }

   // Wait for job termination. Called by client threads
   // --------------------------------------------------
   void WaitForIdle()
      {
      BB->WaitForIdle();
      }

   // -----------------------------------------------
   // This is the thread function executed by all the
   // worker threads during their lifetime.
   // -----------------------------------------------
   void PeerThread(int rk)
      {
      bool my_shut;
      int rank = rk;

      for(;;)
         {
         BB->Wait();        // Here worker threads sleep
            {
            std::lock_guard<std::mutex> lock(pMutex);
            my_shut = shut;
            }
         if(my_shut == false) RunWorkload(rank);
         else break;
         }
      }

   // ----------------------------------------------------
   // This is the function called by worker threads to
   // run a specific job. Worker threads keep trying to
   // deque tasks from their propietary queues and, if
   // propietary queues are empty, they try to steal from
   // another "victim" queue. 
   // The JobMgr JM keeps track of the number of waiting
   // tasks, and is used to stop the recurrent procedure 
   // when there are no more tasks to run
   // --------------------------------------------------
   void RunWorkload(int rk)
      {
      int myrank = rk;
      int victim;
      TLInteger(rk, false);  // store the thread rank
      Task *t;

      // -------------------------------------------------  
      // Enter here an infinite loop where tasks are poped
      // from queues. This infinite loop should work until
      // complete termination is detected.
      // -------------------------------------------------
      while(JM.ActiveTasks())
         {
         t = ws_queue[myrank].try_pop();
         if(t != NULL)
            {
            t->ExecuteTask();
            JM.UnregisterTask();
            }
         else
            {
            // get a victim to steal from
            // --------------------------
            victim = (myrank+1)%nTh;
            if(!ws_queue[victim].isEmpty())
               {
               t = ws_queue[victim].try_steal();
               if(t != NULL)
                  {
                  t->ExecuteTask();
                  JM.UnregisterTask();
                  }
               }
            }
         }
      }

   // -----------------------------------------------------------
   // Called by client threads, before destroying the queue. This
   // is called when the client program does not intend to submit
   // new jobs.
   // -----------------------------------------------------------
   void JoinThreads()
      {
      // set the shut flag
      // ------------------
         {
         std::lock_guard<std::mutex> lock(pMutex);
         shut = true;
         }
      BB->WaitForIdle(); 
      BB->ReleaseThreads();
      for(auto &th : workers) th.join();
      std::cout << "\n Threads joined" << std::endl;
      }

   // end of WSPool class
   };


// *********************************************
// This is the thread function passed to threads
// *********************************************
void threadFunc(void *P, int rank)
   {
   WSPool *tp = (WSPool *)P;
   tp->PeerThread(rank);  // call member function in pool
   }
 

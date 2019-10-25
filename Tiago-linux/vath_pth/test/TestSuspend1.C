// **************************************
// Copyright (c) 2012 Victor Alessandrini
// All rights reserved.
// **************************************
// ***************************************************
// File TestSuspend1.C
// Tests features of the thread pool SPool facility,
// 
// This example launches two tasks that join in a barrier
// before terminating. One of the two tasks spawns a 
// child task before reaching the barrier, and waits for
// it.
//
// This pattern is OK with more than two threads. One
// is at the barrier, the other is waiting for its
// child, we need a third thread to execute the child.
// Otherwise, the code deadlocks.
//
// When, in version 2, we add Suspend_And_Run_Task()
// feature, that allows a thread to suspend the current
// task and dequeue a new task before waiting for its
// childs, the code no longer deadlocks. 
//
// Each task prints the executing thread ID. When there
// are three or more threads in the pool, each task is
// executed by a different thread. When there are only
// two threads, we can see that the child task is executed
// by the same thread that is running the parent task.
//
// This code works fine with version 2, including the
// task suspension facility.
// ***************************************************

#include <stdlib.h>
#include <iostream>
#include <NPool.h>
#include <pthread.h>
#include <Timer.h>
#include <RandInt.h>
#include <Barrier.h>

using namespace std;

RandInt  R(2000);     // generates random integers in [0, 2000]
NPool    *NP;         // reference to ThPool
Barrier  *B;          // reference to Barrier
int      Nth;         // number of threads in the pool

// -------------------------------------------
// These are task classes. They do not get any
// input data, so the constructor is trivial.
// -------------------------------------------
class SpTask: public Task
   {

   public:
    SpTask(): Task() {}       // trivial constructor
    void ExecuteTask()
       {
       Timer T;
       long timewait = (long)R.draw();
       T.Wait(timewait+1000);
       cout << "\n END of child task, executed by thread "
            << pthread_self()  << endl; 
       }
   };


class ParentTask: public Task
   {
   private:
      int rank;

   public:
    ParentTask(int n): Task(), rank(n) {}   // trivial constructor
    void ExecuteTask()
       {
       Timer T;
       T.Wait(500*rank);
       cout <<  " START of main task "  << rank 
            << ", executed by thread " << pthread_self() << endl; 
       if(rank==2)
          {
          // Submit 1 SpawnedTasks, do a taskwait
          // ------------------------------------
          SpTask *t = new SpTask();
          t->SetWaited(true);
          NP->SpawnTask(t);

          // Put a timer here so that eventual idle threads in
          // the pool have time to dequeue the spawned task
          // -------------------------------------------------
          T.Wait(500);

          // Now, wait for child task
          // ------------------------
          NP->TaskWait();
          cout <<  "\n TaskWait returned. Parent task exits " << endl; 
          }
       B->Wait();
       }
   };


int main(int argc, char *argv[])
   {
   int n;
   bool status;
   int groupID;

   if(argc==2) Nth = atoi(argv[1]);
   else Nth = 4;
   std::cout << "\n testing task suspension mechanism in NPool" 
             << std::endl;

   NP = new NPool(Nth, 20);
   B = new Barrier(2);

   // Build the main task, and submit job
   // ------------------------------------
   ParentTask *T1 = new ParentTask(1);
   ParentTask *T2 = new ParentTask(2);
   TaskGroup *TG = new TaskGroup();
   TG->Attach(T1);
   TG->Attach(T2);
   groupID = NP->SubmitJob(TG);
   
   // Wait for parallel jor termination
   // ---------------------------------
   NP->WaitForJob(groupID);
   cout << "\n Parallel job is finished " << endl;
   }  

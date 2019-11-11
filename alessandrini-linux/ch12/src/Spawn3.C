// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File Spawn3.C
// -------------
// Tests features of the thread pool ThPool facility,
// 
// This example tests the correct operation of the task suspension 
// mechanism. When a task decides to wait for children, it checks before 
// waiting if there are pending waiting tasks in the task queue, and if 
// that is the case, it executes one of them. It enters the wait only when
// there are no pending tasks. This mechanism is necessary to avoid
// deadlocks arising from overcommitted threads.
// .
// Two level 0 tasks spawn a child, and wait for termination. This code 
// runs OK with 4 threads in the pool by default. It also runs OK with 
// 3 threads: two threads are waiting for childs, but the third one can 
// dequeue and execute them.
//
// With only 2 threads in the pool, the code deadlocks in the absence of
// a task suspension mechanism: the two threads spawn the child tasks and 
// wait, and there are no other threads in the pool to dequeue and execute 
// them.
//
// Because of the task suspension mechanism implemented in NPool - as well
// as in OpenMP or TB - this code runs correctly on 2 threads.
// **********************************************************************
#include <stdlib.h>
#include <iostream>
#include <NPool.h>
#include <Timer.h>
#include <RandInt.h>

using namespace std;

RandInt  R(2000);     // generates random integers in [0, 2000]
NPool    *NP;         // reference to ThPool
int      Nth;         // number of threads in the pool

// -----------------------------------------------
// These are Task classes: first the spawned tasks
// -----------------------------------------------
class SpTask: public Task
   {
   private:
    int rank;

   public:
    SpTask(int r): Task(), rank(r) {}       // trivial constructor
    void ExecuteTask()
       {
       Timer T;
       long timewait = rank*1000;
       T.Wait(timewait+2000);
       cout << "\n END of spawned " << rank << " task " << endl; 
       }
   };

// Next, the tasks for the initial job
// -----------------------------------
class OuterTask: public Task
   {
   private:
    int rank;

   public:
    OuterTask(int r): Task(), rank(r) {}       // trivial constructor
    void ExecuteTask()
       {
       cout <<  " START of outer task "  << rank << endl; 
   
       // Spawn task, and wait for its termination 
       // Spawned task has same rank as parent task
       // -----------------------------------------
       SpTask *T = new SpTask(rank);
       NP->SpawnTask(T);
       cout <<  "\n Task spawned" << endl;
       NP->TaskWait();
       cout <<  "\n  Outer task " << rank << " exits " << endl; 
       }
   };


int main(int argc, char *argv[])
   {
   int n;
   bool status;
   int groupID;

   if(argc==2) Nth = atoi(argv[1]);
   else Nth = 4;

   NP = new NPool(Nth, 20);

   // Build a Nth task job, and submit job
   // ------------------------------------
   TaskGroup *TG = new TaskGroup();
   for(int n=1; n<=2; ++n) 
      {
      OuterTask *T = new OuterTask(n);
      TG->Attach(T);
      }
   groupID = NP->SubmitJob(TG);
   
   // Wait for parallel jor termination
   // ---------------------------------
   NP->WaitForJob(groupID);
   cout << "\n Parallel job is finished " << endl;
   }  

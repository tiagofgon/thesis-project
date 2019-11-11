// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File Spawn1.C
//
// Testing features of the thread pool NPool facility,
// 
// This example shows how launch a child task, and wait 
// for its termination.
//
// Main() submits a job executing one ParentTask, and
// waits for the job termination.
//
// The ParentTask spawns 4 children, AND TERMINATES,
// not waiting for them
//
// The example shows that the ongoing jo correctly incorporates
// the spawned tasks, and that the job termination is signaled
// when ALL TASKS (children, choldren of children) hhave terminated.
//
// Our "parallel job" construct implements the same features that 
// the OpenMP and TBB taskgroups. 
// **************************************************************

#include <stdlib.h>
#include <iostream>
#include <NPool.h>
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
       T.Wait(timewait+3000);
       cout << "\n END of child task " << endl; 
       }
   };


class ParentTask: public Task
   {
   public:
    ParentTask(): Task() {}   // trivial constructor
    void ExecuteTask()
       {
       cout <<  " START of main task "  << endl; 
      
       // Submit 4 SpawnedTasks, do not wait for termination
       // -----------------------------------------------------
       for(int n=0; n<4; ++n)
          {
          SpTask *t = new SpTask();
          NP->SpawnTask(t, false);
          }
       cout <<  "\n Child tasks posted. Main task exits " << endl; 
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

   // Build the main task, and submit job
   // ------------------------------------
   ParentTask *T = new ParentTask();
   groupID = NP->SubmitJob(T);
   
   // Wait for parallel jor termination
   // ---------------------------------
   NP->WaitForJob(groupID);
   cout << "\n Parallel job is finished " << endl;
   }  

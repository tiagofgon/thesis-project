// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File Spawn1.C
//
// Testing features of the thread pool TaskCentricPool facility,
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
#include <TaskCentricPool.hpp>
#include <Timer.hpp>
#include <RandInt.hpp>
#include <Barrier.hpp>

using namespace std;

RandInt  R(2000);     // generates random integers in [0, 2000]
TaskCentricPool    *NP;         // reference to ThPool
Barrier  *B;          // reference to Barrier
int      Nth;         // number of threads in the pool


void SpTask() {
   Timer T;
   long timewait = (long)R.draw();
   T.Wait(timewait+3000);
   cout << "\n END of child task " << endl; 
}



void ParentTask() {
   cout <<  " START of main task "  << endl; 

   // Submit 4 SpawnedTasks, do not wait for termination
   // -----------------------------------------------------
   for(int n=0; n<4; ++n) {
      Task *t = new Task();
      t->insertTask(SpTask);
      NP->SpawnTask(t, false);
   }
   cout <<  "\n Child tasks posted. Main task exits " << endl; 
}
   
 
int main(int argc, char *argv[]) {
   int n;
   bool status;
   int groupID;

   if(argc==2) Nth = atoi(argv[1]);
   else Nth = 4;

   NP = new TaskCentricPool(Nth, 20);

   // Build the main task, and submit job
   // ------------------------------------
   Task *T = new Task();
   T->insertTask(ParentTask);
   groupID = NP->SubmitJob(T);
   
   // Wait for parallel jor termination
   // ---------------------------------
   NP->WaitForJob(groupID);
   cout << "\n Parallel job is finished " << endl;
   }  

// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File Spawn2.C
// --------------
// Tests features of the thread pool TaskCentricPool facility,
// 
// Same setup as in Spawn1.C, but now the ParentTask
// performs some additional operations. It waits for
// the termination of the initial four children. Then,
// it spawns two more children and waits for them. 
// Finally, it spawns three children and terminates
// (not waiting).
// ***************************************************
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

// ------------------------
// These are Task classes
// ------------------------
// class SpTask: public Task
//    {
//    public:
//     SpTask(): Task() {}       // trivial constructor
//     void ExecuteTask()
//        {
//        Timer T;
//        long timewait = (long)R.draw();
//        T.Wait(timewait+1000);
//        cout << "\n END of child task " << endl; 
//        }
//    };


void SpTask() {
   Timer T;
   long timewait = (long)R.draw();
   T.Wait(timewait+1000);
   cout << "\n END of child task " << endl; 
}
   

// class ParentTask: public Task
//    {
//    public:
//     ParentTask(): Task() {}   // trivial constructor
//     void ExecuteTask()
//        {
//        cout <<  " START of main task "  << endl; 
      
//        // Submit 4 SpawnedTasks, and wait for termination
//        // -----------------------------------------------
//        for(int n=0; n<4; ++n)
//           {
//           SpTask *t = new SpTask();
//           NP->SpawnTask(t);
//           }
//        cout <<  "\n Four child tasks posted " << endl; 
//        NP->TaskWait();
//        cout <<  "\n Four child tasks ended. Posting two more tasks" << endl;
       
//        for(int n=0; n<2; ++n)
//           {
//           SpTask *t = new SpTask();
//           NP->SpawnTask(t);
//           }
//        cout <<  "\n Two child tasks posted. Waiting for them " << endl; 
//        NP->TaskWait();
//        cout <<  "\n Two tasks ended." 
//             <<  "\n Three more tasks go, not taskwaited" << endl;
//       for(int n=0; n<3; ++n)
//           {
//           SpTask *t = new SpTask();
//           NP->SpawnTask(t, false);
//           }
//        cout <<  "\n Parent task ended " << endl; 
//        }
//    };


void ParentTask() {
   cout <<  " START of main task "  << endl; 

   // Submit 4 SpawnedTasks, and wait for termination
   // -----------------------------------------------
   for(int n=0; n<4; ++n)
      {
      // SpTask *t = new SpTask();
      // NP->SpawnTask(t);
      Task *t = new Task();
      t->insertTask(SpTask);
      NP->SpawnTask(t);
      }
   cout <<  "\n Four child tasks posted " << endl; 
   NP->TaskWait();
   cout <<  "\n Four child tasks ended. Posting two more tasks" << endl;
   
   for(int n=0; n<2; ++n)
      {
      // SpTask *t = new SpTask();
      // NP->SpawnTask(t);
      Task *t = new Task();
      t->insertTask(SpTask);
      NP->SpawnTask(t);
      }
   cout <<  "\n Two child tasks posted. Waiting for them " << endl; 
   NP->TaskWait();
   cout <<  "\n Two tasks ended." 
        <<  "\n Three more tasks go, not taskwaited" << endl;
   for(int n=0; n<3; ++n)
      {
      // SpTask *t = new SpTask();
      // NP->SpawnTask(t, false);
      Task *t = new Task();
      t->insertTask(SpTask);
      NP->SpawnTask(t);
      }
   cout <<  "\n Parent task ended " << endl; 
}



int main(int argc, char *argv[])
   {
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

// **************************************
// Copyright (c) 2012 Victor Alessandrini
// All rights reserved.
// **************************************
// ***************************************************
// File Spawn2.C
// Tests features of the thread pool ThPool facility,
// 
// This example shows how launch a child task, and wait 
// for its termination
// ***************************************************

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
class SpSpTask: public Task
   {

   public:
    SpSpTask(): Task() {}       // trivial constructor
    void ExecuteTask()
       {
       Timer T;
       long timewait = (long)R.draw();
       T.Wait(timewait+1000);
       cout << "\n END SPSP of child task "  << endl; 
       }
   };

class SpTask: public Task
   {

   public:
    SpTask(): Task() {}       // trivial constructor
    void ExecuteTask()
       {
       Timer T;
       long timewait = (long)R.draw();
       T.Wait(timewait+1000);
       SpSpTask *t = new SpSpTask();
       NP->SpawnTask(t);
       NP->TaskWait();
       cout <<  "\n TaskWait returned. Sp task exits " << endl; 
       }
   };


class ParentTask: public Task
   {
   public:
    ParentTask(): Task() {}   // trivial constructor
    void ExecuteTask()
       {
       cout <<  " START of main task "  << endl; 
      
       // Submit 1 SpawnedTasks, do a taskwait
       // ------------------------------------
       SpTask *t = new SpTask();
       t->SetWaited(true);
       NP->SpawnTask(t);
       cout <<  "\n ExNewTask returned. Calling TaskWait" << endl; 
       NP->TaskWait();
       cout <<  "\n TaskWait returned. Parent task exits " << endl; 
       }
   };


int main(int argc, char *argv[])
   {
   int n;
   bool status;
   int groupID;

   if(argc==2) Nth = atoi(argv[1]);
   else Nth = 4;
   std::cout << "\n Testing the task spawn mechanism " << std::endl;

   NP = new NPool(Nth, 20);

   // Build the main task, and submit job
   // ------------------------------------
   ParentTask *T1 = new ParentTask();
   groupID = NP->SubmitJob(T1);
   
   // Wait for parallel jor termination
   // ---------------------------------
   NP->WaitForJob(groupID);
   cout << "\n Parallel job is finished " << endl;
   }  

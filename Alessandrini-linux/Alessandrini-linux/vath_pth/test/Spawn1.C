// **************************************
// Copyright (c) 2012 Victor Alessandrini
// All rights reserved.
// **************************************
// ***************************************************
// File TestSuspend1.C
// Tests features of the thread pool NPool facility,
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
class SpTask: public Task
   {

   public:
    SpTask(): Task() {}       // trivial constructor
    void ExecuteTask()
       {
       Timer T;
       long timewait = (long)R.draw();
       T.Wait(timewait+1000);
       cout << "\n END of child task "  << endl; 
       }
   };


class ParentTask: public Task
   {
   private:
     int rank;

   public:
    ParentTask(int R): Task(), rank(R) {}   // trivial constructor
    void ExecuteTask()
       {
       cout <<  " START of main task " << rank << endl;
       if(rank==2)
           {
           // Spawn a task      
           SpTask *t = new SpTask();
           t->SetWaited(true);
           NP->SpawnTask(t);
           NP->TaskWait();
           }
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
   std::cout << "\n Testing the task spawn mechanism " << std::endl;

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

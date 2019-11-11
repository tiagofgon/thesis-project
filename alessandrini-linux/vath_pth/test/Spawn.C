// **************************************
// Copyright (c) 2015 Victor Alessandrini
// All rights reserved.
// **************************************
// ***************************************************
// File Spawn.C
// Tests basic taskwait mechanism,
// 
// This example shows how launch a child task, and wait 
// for its termination.
//
// *) Main launches the parent task in a four thread
//    pool. Three threads are idle
// *) Parent task spawns nTask tasks, waits for 1/2 second 
//    and calls taskwait. Waiting before taskwaiting lets
//    the idle threads execute the childs if available
// *) Default nTasks=2, and in this case there is no
//    need of tasks suspension.
// *) Increasing the number of tasks via the command
//    line overcommits threads, and enforces the task
//    suspension mechanism.
//
//    4 Threads
//    1 parent task, that submits nTasks tasks,
//    suspends 500ms, and waits for all childs.
// ***************************************************

#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <NPool.h>
#include <Timer.h>
#include <SafeCout.h>

using namespace std;

NPool    *NP;         // reference to ThPool
int      Nth;         // number of threads in the pool
SafeCout SC;          // to order output to stdout
int      nTasks;

class SpTask: public Task
   {
   public:
	SpTask() : Task() { }       // trivial constructor
    void ExecuteTask()
       {
       std::ostringstream os;
       Timer T;
	   int tid = GetTaskID();
       os << "\n Starting child " << tid;
       SC.Flush(os);
       T.Wait(500);
       os <<  "\n Exiting child "  << tid;
       SC.Flush(os); 
       }
   };


class ParentTask: public Task
   {
   public:
    ParentTask(): Task() {}   // trivial constructor
    void ExecuteTask()
       {
       Timer T;
       std::ostringstream os;
	   int tid = GetTaskID();
       os <<  " Task " << tid << " spawns several child tasks "; SC.Flush(os); 
      
       // Submit 1 SpawnedTasks, do a taskwait
       // ------------------------------------
	   for (int n = 1; n <= nTasks; ++n)
	       {
		   SpTask *t = new SpTask();
		   NP->SpawnTask(t);
	       }
       T.Wait(2000);
       os <<  "\n Task " << tid << " calls TaskWait"; SC.Flush(os); 
       NP->TaskWait();
       os << "\n TaskWait returned. Parent task exits ";
       SC.Flush(os); 
       }
   };


int main(int argc, char *argv[])
   {
   int groupID;
   std::ostringstream os;

   if (argc == 2) nTasks = atoi(argv[1]);
   else nTasks = 1;

   NP = new NPool(2, 20);
   os << "\n Testing the basic taskwait mechanism ";
   SC.Flush(os);

   // Build the main task, and submit job
   // ------------------------------------
   ParentTask *T1 = new ParentTask();
   groupID = NP->SubmitJob(T1);
   
   // Wait for parallel jor termination
   // ---------------------------------
   NP->WaitForJob(groupID);
   os << "\n Parallel job is finished "; SC.Flush(os);
   }  

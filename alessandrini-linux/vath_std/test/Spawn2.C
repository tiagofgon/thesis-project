// **************************************
// Copyright (c) 2012 Victor Alessandrini
// All rights reserved.
// **************************************
// ***************************************************
// File Spawn2.C
// Tests features of the thread pool ThPool facility,
// 
// A parent task spawns a child task, which in turn
// spawns a child task.
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
       std::ostringstream os;
       Timer T;
       os << "\n Entering SpSpTask";
       SC.Flush(os);
       T.Wait(1750);
       os << "\n SpSp task exits ";
       SC.Flush(os); 
       }
   };

class SpTask: public Task
   {
   public:
    SpTask(): Task() {}       // trivial constructor
    void ExecuteTask()
       {
       std::ostringstream os;
       Timer T;
       os << "\n Entering SpTask";
       SC.Flush(os);
       T.Wait(2000);
       SpSpTask *t = new SpSpTask();
       NP->SpawnTask(t);
       NP->TaskWait();
       os <<  "\n Sp task exits ";
       SC.Flush(os); 
       }
   };


class ParentTask: public Task
   {
   public:
    ParentTask(): Task() {}   // trivial constructor
    void ExecuteTask()
       {
       std::ostringstream os;
       os <<  " START of main task "; SC.Flush(os); 
      
       // Submit 1 SpawnedTasks, do a taskwait
       // ------------------------------------
       SpTask *t = new SpTask();
       NP->SpawnTask(t);
       //os <<  "\n Parent task calling TaskWait"; SC.Flush(os); 
       NP->TaskWait();
       os << "\n Parent task exits ";
       SC.Flush(os); 
       }
   };


int main(int argc, char *argv[])
   {
   int groupID;
   std::ostringstream os;

   if(argc==2) Nth = atoi(argv[1]);
   else Nth = 4;

   NP = new NPool(Nth, 20);
   std::cout << "\n *** Third test of spawn mechanism\n" << std::endl;

   // Build the main task, and submit job
   // ------------------------------------
   ParentTask *T1 = new ParentTask();
   groupID = NP->SubmitJob(T1);
   
   // Wait for parallel jor termination
   // ---------------------------------
   NP->WaitForJob(groupID);
   os << "\n Parallel job is finished "; SC.Flush(os);
   }  

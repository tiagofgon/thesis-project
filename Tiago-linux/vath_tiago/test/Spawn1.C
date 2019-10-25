// **************************************
// Copyright (c) 2012 Victor Alessandrini
// All rights reserved.
// **************************************
// ***************************************************
// File TestSuspend1.C
// Tests features of the thread pool NPool facility,
// 
// This example shows how to spawn a child task, and wait 
// for its termination.
//
// WHAT THIS EXAMPLE SHOWS.
// -----------------------
// A pool of Nth threads is launched, with a parent task
// per thread, having a rank in [1, Nth] passed via the
// parent task constructor.
// Parent tasks 1 and 2 spawn a child task and wait for
// its termination.
//
// All the parent tasks are synchronized at a barrier 
// at termination. The object of the barrier is to keep
// all the threads busy so that the ones that are not
// waiting for childs cannot "help" in running the
// child tasks.
//
// This forces the taskwaiting threads to run themselves
// the child tasks before entering the taskwait.
// This code is a test of the task suspension mechanism
// that avoids deadlocks when the threads are overcommitted.
// ********************************************************

#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <NPool.h>
#include <Timer.h>
#include <Barrier.h>
#include <SafeCout.h>

using namespace std;

NPool    *NP;         // reference to ThPool
Barrier  *B;          // reference to Barrier
int      Nth;         // number of threads in the pool
SafeCout SC;          // orders output to stdout

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
       std::ostringstream os;
       T.Wait(1500);
       os << "\n END of spawned task ";
       SC.Flush(os);
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
       std::ostringstream os;
       os <<  " START of main (parent) task " << rank; 
       SC.Flush(os);
       if(rank<3)
           {
           // Spawn a task      
           SpTask *t = new SpTask();
           NP->SpawnTask(t);
           NP->TaskWait();
           os << "\n Rank " << rank << " thread finished taskwait";
           SC.Flush(os);
           }
       B->Wait();
       }
   };


int main(int argc, char *argv[])
   {
	std::ostringstream os;
   int groupID;
   if(argc==2) Nth = atoi(argv[1]);
   else Nth = 4;

   NP = new NPool(Nth, 20);
   B = new Barrier(Nth);
   std::cout << "\n *** Second test of spawn mechanism\n" << std::endl;

   // Build the main task, and submit job
   // ----------------------------------
   TaskGroup *TG = new TaskGroup();
   for (int n = 1; n <= Nth; ++n)
      {
      ParentTask *t = new ParentTask(n);
      TG->Attach(t);
      }
   groupID = NP->SubmitJob(TG);
   
   // Wait for parallel jor termination
   // ---------------------------------
   NP->WaitForJob(groupID);
   os << "\n Parallel job is finished ";
   SC.Flush(os);
   delete B;
   }  

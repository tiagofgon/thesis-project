// **************************************
// Copyright (c) 2012 Victor Alessandrini
// All rights reserved.
// **************************************
// File TPool1.C
// -------------
// Tests the general features of the NPool
// pool facility, including job status 
// information. 
// ---------------------------------------

#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <NPool.h>
#include <Timer.h>
#include <RandInt.h>

using namespace std;

RandInt  R(1000);     // generates random integers in [0, 1000]

// -----------------------------------------
// Auxiliary function, to print a job status
// -----------------------------------------
void ReportStatus(bool S)
   { 
   if(S) cout << "\n Job is queued or running" << endl;
   else cout << "\n Job is finished" << endl;
   }

// ----------------------------------------------------
// This is a task class. It will be used for all the
// tasks submitte to the pool in this application.
//
// The task receives an integer argument "rank" that 
// identifies the task.
//
// All the task does is to print an identifying start 
// message, waits for some random period of time between
// 0 and 1000 milliseconds, print an identifying end 
// message, and exit. 
//
// We use the SafeCout utility to implement exclusive
// access to the screen and avoid mixing the thread
// output
// ----------------------------------------------------
class TestTask: public Task
   {
   private:
    int rank;
    long timewait;
    Timer T;
    ostringstream os;

   public:
   TestTask(int r): Task(), rank(r) {}

   void ExecuteTask()
      {
      // --------------------------------
      cout << " TASK " << rank << " START" << endl; 
      // --------------------------------
      int ir = R.draw();          // random integer in [0, 1000]
      timewait = (long) (ir);
      T.Wait(timewait);
      // -------------------------------
      cout << " TASK " << rank << " END" << endl; 
      // -------------------------------
      }
   };


int main(int argc, char *argv[])
   {
   int n;
   bool status;
   int taskID[6];
   int jobID[3];

   NPool NP(2, 20);
   std::cout << "\n *** Testing basic NPool services\n " << std::endl;

   // -----------------------------------------------
   // Post six independent one task jobs to the pool, 
   // passing a rank identifier, and collect job IDs
   // -----------------------------------------------
   cout << " --- Main: submitting six one task jobs" << endl;
   for (n=0; n<6; n++) 
      {
      TestTask *t = new TestTask(n);
      taskID[n] = NP.SubmitJob(t);
      }
   cout << " --- Main: six one task jobs submitted" << endl;
   
   // Ask for status of taskID[3]
   // ---------------------------
   cout << " --- Main: asking for status of job 3" << endl;
   status = NP.JobStatus(taskID[3]);
   cout << "checkpoint 1 " << endl;
   ReportStatus(status);

   // Build a group of three tasks, with rank IDs
   // 10, 11, 12, and submit it
   // -----------------------------------------
   TaskGroup *TG = new TaskGroup();
   cout << " --- Main: submitting one job with three tasks\n"
        << "     and waiting for termination " << endl;
   for (n=10; n<13; n++)
      {
      TestTask *t = new TestTask(n);
      TG->Attach(t);
      }
   jobID[0] = NP.SubmitJob(TG);
   
   // Wait for parallel jor termination
   // ---------------------------------
   NP.WaitForJob(jobID[0]);
   cout << " --- Main: three task parallel job in finished "
        << endl; 

   NP.WaitForIdle();
   cout << " --- Main: pool is now idle \n";
   cout << " --- Main: submit again 6 single task jobs" << endl;

   for (n=0; n<6; n++) 
      {
      TestTask *t = new TestTask(n);
      taskID[n] = NP.SubmitJob(t);
      }

   // Ask for status of taskID[4]
   // ---------------------------
   status = NP.JobStatus(taskID[4]);
   ReportStatus(status);
   NP.WaitForIdle();
   cout << " --- Main: pool is again idle " << endl;
   }  

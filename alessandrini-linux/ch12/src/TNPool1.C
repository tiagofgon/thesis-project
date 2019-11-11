// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File NPool1.C
// -------------
// Tests the very general features of the NPool facility,
// including job status information. 
// ------------------------------------------------------

#include <stdlib.h>
#include <iostream>
#include <NPool.h>
#include <Timer.h>
#include <RandInt.h>
#include <SafeCout.h>

using namespace std;

RandInt  R(1000);     // generates random integers in [0, 1000]
SafeCout SC;          // for screen output

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
// tasks submitted to the pool in this example
//
// The task receives an integer identifier "rank".
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
      os << " TASK " << rank << " START"; 
      SC.Flush(os); 
      // --------------------------------
      int ir = R.draw();          // random integer in [0, 1000]
      timewait = (long) (ir);
      T.Wait(timewait);
      // -------------------------------
      os << " TASK " << rank << " END"; 
      SC.Flush(os); 
      // -------------------------------
      }
   };


int main(int argc, char *argv[])
   {
   int n;
   bool status;
   int taskID[6];
   int jobID[3];
   ostringstream os;

   NPool NP(2, 20);    // two threads in pool

   // -----------------------------------------------
   // Post six independent one task jobs to the pool, 
   // passing a rank identifier, and collect job IDs
   // -----------------------------------------------
   os << " --- Main: submitting six one task jobs";
   for (n=0; n<6; n++) 
      {
      TestTask *t = new TestTask(n);
      taskID[n] = NP.SubmitJob(t);
      }
   SC.Flush(os);
   
   // Ask for status of taskID[3]
   // ---------------------------
   os << " --- Main: asking for status of job 3";
   status = NP.JobStatus(taskID[3]);
   ReportStatus(status);

   // Build a group of three tasks, with rank IDs
   // 10, 11, 12, and submit it
   // -----------------------------------------
   TaskGroup *TG = new TaskGroup();
   os << " --- Main: submitting one job with three tasks\n"
      << "     and waiting for termination ";  SC.Flush(os);
   for (n=10; n<13; n++)
      {
      TestTask *t = new TestTask(n);
      TG->Attach(t);
      }
   jobID[0] = NP.SubmitJob(TG);
   
   // Wait for parallel jor termination
   // ---------------------------------
   NP.WaitForJob(jobID[0]);
   os << " --- Main: three task parallel job in finished "; SC.Flush(os);

   NP.WaitForIdle();
   os << " --- Main: pool is now idle \n";
   os << " --- Main: submit again 6 single task jobs";
   SC.Flush(os);

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
   os << " --- Main: pool is again idle "; SC.Flush(os);
   }  

// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
/* TNpool2.C
 * Tests the barrier behavior, when the number of tasks
 * is different from the number of threads.
 *
 * The main function launches a parallel job of 6 tasks,
 * calling a barrier.
 *
 * The number of worker threads is by default 6, but a
 * different number can be passes by the command line.
 *
 * With less than 6 worker threads, the code deadlocks
 * ---------------------------------------------------- 
 */

#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <NPool.h>
#include <Timer.h>
#include <RandInt.h>
#include <Barrier.h>
#include <SafeCout.h>

using namespace std;

RandInt  R(3000);     // generates random integers in [0, 1000]
Barrier  *B;          // reference to Barrier
NPool    *TP;
SafeCout SC;          // for safe IO on stdout


// ---------------------------------------------------------
// This is a task class. It will be used for all the tasks
// tasks submitted to the pool in this application
//
// All the task does is to print an identifying start message,
// wait for some random period of time, call a barrier wait, 
// print again identifying message, and exit. 
// --------------------------------------------------------
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
      int retval;
      os << " TASK " << rank << " START";
      SC.Flush(os); 
      int ir = R.draw();          // random integer in [0, 1000]
      timewait = (long) (rank*ir);
      T.Wait(timewait);
      retval = B->Wait();                  // barrier wait
      os << " TASK " << rank << " END";
      if(retval<0)                         // this is last thread
         os << "\n ----------------------- \n";
      SC.Flush(os); 
      }
   };


int main(int argc, char *argv[])
   {
   int n, Nth;
   int groupID;

   if(argc==2) Nth = atoi(argv[1]);
   else Nth = 6;

   TP = new NPool(Nth);
   B  = new Barrier(Nth);      // barrier for 6 tasks

   // Build a group of 6 tasks, and submit
   // ------------------------------------
   TaskGroup *TG = new TaskGroup();
   for (n=1; n<=6; n++)
      {
      TestTask *t = new TestTask(n);
      TG->Attach(t);
      }
   groupID = TP->SubmitJob(TG);
   
   // Wait for parallel jor termination
   // ---------------------------------
   TP->WaitForJob(groupID);
   cout << "Parallel job in finished " << endl;
   }  

// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File Nested.C
//
// Tests features of the thread pool NPool facility,
// 
// This example shows how to implement inner nested
// jobs using a new, inner NPool object.
// ***************************************************

#include <stdlib.h>
#include <iostream>
#include <NPool.h>
#include <Timer.h>
#include <RandInt.h>
#include <Barrier.h>

using namespace std;

RandInt  R(2000);     // generates random integers in [0, 2000]
NPool    *NP;         // reference to NPool object
Barrier  *B;          // reference to Barrier
int      Nth;         // number of threads in the pool

// ---------------------------------------------
// This task class is the task performed by the
// nested parallel job.
// Task gets an integer identifier as constructor argument.
// It prints a start message, waits for a random time duration,
// and prints an end message 
// -----------------------------------------------------------
class NestedTask: public Task
   {
   private:
    int rank;
    
   public:
    NestedTask(int r): Task(), rank(r) {}
    void ExecuteTask()
       {
       long timewait;
       Timer T;
       timewait = (long)R.draw();
       cout <<  " START of inner task " << rank << endl; 
       T.Wait(timewait+2000);
       cout << " END of inner task " << rank << endl; 
       }
   };

// ------------------------------------------------
// This is the task of the outer parallel job.
// One of the tasks creates a new NPool pool on
// the fly and submits the nested job. The inner
// NPool pool is local, and dissappears when the
// task terminates
// ----------------------------------------------
class OuterTask: public Task
   {
   private:
    int rank;
    
   public:
    OuterTask(int r): Task(), rank(r) {}
    void ExecuteTask()
       {
       cout <<  " START of Task " << rank << endl;
       // ---------------------------------------------------------
       if(rank==3)
          {
          // ------------------------------------------------------
          // Creation of new task pool, creation of task group
          // of four nested tasks, submission, wait for termination,
          // and implicit destruction of the new task pool
          // -----------------------------------------------------
          NPool INP(4, 10);     // inner thread pool      
          TaskGroup *TG = new TaskGroup();
          for(int n=0; n<4; n++)
             {
             NestedTask *t = new NestedTask(n);
             TG->Attach(t);
             }
          int ID = INP.SubmitJob(TG);
          INP.WaitForJob(ID);
          }
       // --------------------------------------------------------
       B->Wait();
       cout << " END of task " << rank << endl; 
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
   B  = new Barrier(4);      // barrier for 4 outer tasks

   // Build a group of for tasks, and post it
   // -----------------------------------------
   TaskGroup *TG = new TaskGroup();
   for (n=0; n<4; n++) 
      {
      OuterTask *t = new OuterTask(n);
      TG->Attach(t);
      }
   groupID = NP->SubmitJob(TG);

   // Wait for parallel jor termination
   // ---------------------------------
   NP->WaitForJob(groupID);
   cout << "Parallel job in finished " << endl;
   }  

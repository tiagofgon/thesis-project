// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File Nested1.C
// Tests the features of the thread pool ThPool facility.
// 
// This example is the same as Nested.C, but now the nested
// job is submitted to THE SAME pool, which is not acceptable.
// Executing the example shows that the nested job submission 
// is rejected.
// ***************************************************

#include <stdlib.h>
#include <iostream>
#include <NPool.h>
#include <Timer.h>
#include <RandInt.h>
#include <Barrier.h>

using namespace std;

RandInt  R(3000);     // generates random integers in [0, 3000]
NPool    *NP;         // reference to ThPool
Barrier  *B;          // reference to Barrier
int      Nth;         // number of threads in the pool

// ------------------------
// These are Task classes 
// ------------------------

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


class OuterTask: public Task
   {
   private:
    int rank;
    
   public:
    OuterTask(int r): Task(), rank(r) {}
    void ExecuteTask()
       {
       cout <<  " START of Task " << rank << endl; 
       if(rank==1)
          {
          // construct new task group, submit nested job,
          // and wait for its termination
          // --------------------------------------------
          TaskGroup *TG = new TaskGroup();
          for(int n=0; n<2; n++)
             {
             NestedTask *t = new NestedTask(n);
             TG->Attach(t);
             }
          int ID = NP->SubmitJob(TG);
          NP->WaitForJob(ID);
          cout << "\nNested job is terminated" << endl;
          }
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
   else Nth = 8;

   NP = new NPool(Nth, 20);
   B  = new Barrier(2);      // barrier for 4 outer tasks

   // Build a group of two tasks, and post it
   // -----------------------------------------
   TaskGroup *TG = new TaskGroup();
   for (n=0; n<2; n++) 
      {
      OuterTask *t = new OuterTask(n);
      TG->Attach(t);
      }
   groupID = NP->SubmitJob(TG);
   
   // Wait for parallel job termination
   // ---------------------------------
   NP->WaitForJob(groupID);
   //NP->WaitForIdle();    // THIS WORKS
   cout << "Parallel job in finished " << endl;
   }  

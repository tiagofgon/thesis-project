// File TPreorder2.C
// -----------------------------------------------------------------
// Testing the spawning of a large number of tasks, to try to
// understant the Preorder bug.
// Modify TPreorder1
// ----------------------------------------------------------------

#include <iostream>
#include <stdio.h>
#include <NPool.h>
#include <math.h>
#include <CpuTimer.h>
#include <SafeCout.h>
#include <SafeCounter.h>
#include <Rand.h>
#include <RandInt.h>

using namespace std;

// Global variables
// ----------------
int          nTh;
NPool        *NP;
SafeCout     SC;
SafeCounter  Count;
SafeCounter  CC;
Rand         Rglobal(999);
RandInt      RI(3);
const double EPS = 0.0000001;
int          nTasks;

class UpdateValue : public Task
   { 
   private:
     double value;
     int    id;
     int    layer;
     Rand   *MyRand;
     std::ostringstream os;

   public:
    UpdateValue(int rank, int deep) : Task()
       {
       value    = Rglobal.draw();
       int seed = Count.Next();
       id       = rank;
       layer    = deep;
       MyRand   = new Rand(99*seed);
       os << "\n Running task number " << id ;
       }

    ~UpdateValue() { delete MyRand; }

    void ExecuteTask()
       {
       double d;
       int n, nspawn;

       SC.Flush(os);
       do
          {
          d = MyRand->draw();
          }while(fabs(d-value)>EPS);

       if(layer<3)
          { 
          nspawn = RI.draw();
          for(n=0; n<nspawn; ++n)
             {
             UpdateValue *T = new UpdateValue(id+(n+1)*nTasks, layer+1);
             NP -> SpawnTask(T, false);
             } 
          }
       }
    };        

// Auxiliary function that does the job
// ------------------------------------
void TaskLauncher(int nTasks)
   {
   int n, jobid, rank;

   // First, create a TaskGroup for the update of the
   // root tasks
   // ----------------------------------------------
   TaskGroup *TG = new TaskGroup();
   for(n=0; n<nTasks; ++n)
      {
      rank = CC.Next();
      UpdateValue *T = new UpdateValue(rank, 0);
      TG->Attach(T);
      }

   // Sublit job and wait for it
   // --------------------------
   jobid = NP->SubmitJob(TG);
   NP->WaitForJob(jobid);
   }

// The main function
// -----------------

int main(int argc, char **argv)
   {
   int n, jobid;
   CpuTimer T;
   int nTh, nSwaps;
   int target;

   // Get command line input
   // ----------------------
   nTh = 4;
   nTasks = 500;
   nSwaps = 2;
   if(argc==2) nTh = atoi(argv[1]);
   if(argc==3)
      { 
      nTh = atoi(argv[1]);
      nTasks = atoi(argv[2]);
      }
   if(argc==4)
      { 
      nTh = atoi(argv[1]);
      nTasks = atoi(argv[2]);
      nSwaps = atoi(argv[3]);
      }

   NP = new NPool(nTh);

   std::cout << "\n Configuration :"
             << "\n -------------- "
             << "\n Number of threads " << nTh
             << "\n Number of initial tasks   " << nTasks
             << "\n Traversals        " << nSwaps << std::endl;
 
   // Do the traversal 
   // ----------------
   T.Start();
   for(unsigned int trial=0; trial<nSwaps; ++trial)
      {
      TaskLauncher(nTasks);
      }
   T.Stop();
   T.Report();
   }


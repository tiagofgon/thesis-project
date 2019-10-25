// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File Unbalance.C
// ----------------------------------------------------
// This is a simple, embarassingly parallel application,
// running  Ntk irregular tasks (the total computation
// of each task is fluctuating). The purpose of this
// example is to run the tasks on Nth threads (Nth<Ntk)
// and verify that a very good parallel speedup is obtained
//
// This example is a simpler version of ForEach.C 
// ----------------------------------------------------

#include <iostream>
#include <stdio.h>
#include <Rand.h>
#include <NPool.h>
#include <math.h>
//#include <TimeReport.h>dd
#include <SafeCounter.h>
#include <SafeCout.h>

using namespace std;

// Global variables
// ----------------
int  nTh;
int  Ntk;
float precision;
SafeCounter SC;
SafeCout    Scout;

void InputData();

// -------------------------------------------------------
// This task will be initialized with a uniform deviate in
// [0,1] when the task is created. This random number comes
// from a global random number generator that initializes
// all tasks.
//
// When executed, the task keeps calling its own, internal
// generator until a value is obtained equal to the initial
// value, within a given precision.
//
// Decreasing the precision increases the work done by each
// task.
// -------------------------------------------------------
class SearchRandom : public Task
   { 
   private:
     int rank;
     double target, precision;
     ostringstream os;

   public:
    SearchRandom(int r, double t, double pr) : Task(), 
                 rank(r), target(t), precision(pr) {}

    void ExecuteTask()
       {
       Rand R(999 * rank);
       double x;
       do
          {
          x = R.draw();
          }while( fabs(x-target)>precision );
       }
   };


// The main function
// -----------------

int main(int argc, char **argv)
   {
   int n, jobid;
   //TimeReport T;
   Rand rd(777);     // to initialize tasks

   InputData();
   std::cout << "\n Input parameters are: " << nTh << " threads, " 
             << Ntk << " tasks, precision = " << precision << std::endl;

   // Construct a parallel job
   // ------------------------
   NPool NP(nTh);
   TaskGroup *TG = new TaskGroup();
   for(n=1; n<=Ntk; ++n)
      {
      double target = rd.draw();
      SearchRandom *T = new SearchRandom(n, target, precision);
      TG->Attach(T);
      }
      
   // Run the parallel job
   // --------------------
   jobid = NP.SubmitJob(TG);
   NP.WaitForJob(jobid);
   }

// ---------------------------------------------------
// This function reads from file "unbal.dat" the number
// of threads Nth, the number of tasks Ntk, and the 
// precision. 
// --------------------------------------------------
void InputData()
    {
    FILE *fp;
    char buffer[128];

    if( !(fp = fopen("unbal.dat", "r") ))
	   {
	   cout << "\n Input error" << endl;
	   exit(0);
	   }
	
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &nTh);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &Ntk);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%f", &precision);
    fclose(fp);
    }


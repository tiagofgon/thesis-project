// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// AreaGroupBis.C
// --------------
//
// This is a TBB derived task class that performs the parallel 
// calculation of the area below a function that receives and 
// returns a double.
//
// STRATEGY:
//
// A more sophisticated architecture of the "AreaGroup.C" example.
// 
// The computing part is the same: Nt SPMD tasks are launched to compute
// the area of a sub-domain, and store partial results in a Reduction<double>
// object. as childs of an empty_task. 
//
// As before, all the tasks are then spawned at the same level. But now
// they are children of an empty task that encapsulates the entire parallel
// job.
//
// This leads to a "job submission" style where main launches the via the
// empty task, does other things if needed, and when the time comes wait
// for the termination of the parallel job, via the empty task.
//
// **********************************************************************
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <Reduction.h>
#include <iostream>
#include "tbb/task_scheduler_init.h"
#include "tbb/tbb_stddef.h"
#include "tbb/task.h"

using namespace tbb;

void InputData();
double Area(double a, double b, double(*func)(double))
   {
   int n, i, j;
   double s, snew, x, tnm, sum, del;

   n = 1;
   i = 1; 
   snew = 0.5*(b-a)*(func(a)+func(b));
   do
     {
     s = snew;
     i <<=1;
     tnm = i;
     del = (b-a)/tnm;
     x = a+0.5*del;
     for(sum=0.0, j=1; j<=i; j++, x+=del) sum += func(x);
     snew = 0.5*(s+(b-a)*sum/tnm);
     }while(fabs(s-snew)>0.000001);
    
   return snew;
   }       

// Global variables
// ---------------- 
double FCT(double x)            // function to integrate
   {
   return sin(x);
   }

int    Nth;
int    Ntasks;
Reduction<double> RD;           // cumulates partial results    


// --------------------------------------------------------------
// This is an auxiliary function, to submit a SPMD job described
// by a task_list.
//
// This function:
// 1 - allocates an empty task, called root, that will be the 
//     parent (sucessor) task of the Nt SPMD tasks
// 2 - sets the root reference count to Nt+1
// 3 - The SPMD tasks are allocated as childs of root, and
//     spawned
// 4 - The function returns a pointer to the root task
//
// Since the root task is the successor of the SPMD tasks, 
// it will just wait for their termination, and it is not
// executed, because its reference count has not reached 
// zero.
//
// The main thread calling this function will wait for the
// termination of of the root task 
// --------------------------------------------------
template<typename T>
empty_task* SubmitSpmdJob(int ntasks)
   {
   empty_task* parent = new( task::allocate_root() ) empty_task;
   parent->set_ref_count(ntasks+1);
   for(int k=1; k<=ntasks; ++k)
      {
      T *t = new( parent->allocate_child() ) T(0, 1, k);
      task::spawn(*t);
      }
   return parent;
   }

// -----------------------------------------------
// Auxiliary function to wait for job submitted
// by previous function. Notice that we explicitly
// destroy the root task, because it has never been
// executed.
// ---------------------------------------
void WaitForJob(empty_task* parent)
   {
   parent->wait_for_all();
   parent->destroy(*parent);
   }

// --------------
// The task class
// --------------
class AreaTask : public task
   {
   public:
   double A, B;     //  global limits
   int    rank;     // rank of this task, in [1, Ntasks]

   AreaTask(double a, double b, int r): A(a), B(b), rank(r) {}

   task *execute()
      {
      double a, b, strip, retval;
      // Before we start, produce information about this task
      // ---------------------------------------------------------
      int rf = ref_count();
      std::cout << "\nExecuting thread : " << pthread_self()
                << "\n Task (" << a << ", " << b  << ")" 
                << std::endl;
      // ---------------------------------------------------------
      strip = (B-A)/Ntasks;
      a = (double)(rank-1)*strip;
      b = (double)rank*strip;
      retval = Area(a, b, FCT);
      RD.Accumulate(retval);
      return NULL;
      }
   }; 


int main(int argc, char **argv)
   {
   double area;
   InputData();     // read Nth and Ntasks 
   task_scheduler_init init(Nth);
   empty_task *parent = SubmitSpmdJob<AreaTask>(Ntasks);
   // --------------------------------------
   // here, main can do other work if needed
   // --------------------------------------
   WaitForJob(parent); 
   area = RD.Data();
   std::cout << "\nArea is : " << area << std::endl;
   }

// Auxiliary I/O function
// ----------------------
void InputData()
    {
    FILE *fp;
    char buffer[128];

    if( !(fp = fopen("areath.dat", "r") ))
	   {
	   std::cout << "\n Input error" << std::endl;
	   exit(0);
	   }
	
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &Nth);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &Ntasks);
    fclose(fp);
    }
 




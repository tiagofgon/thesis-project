// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// AreaGroup.C
// ----------
// This is a TBB derived task class that will perform the parallel 
// calculation of the area below a function that receives and returns a 
// double.
//
// STRATEGY:
// --------
// The main() function submits a list of Nt SPMD root tasks, and waits for
// their termination. All the computing tasks are then spawned in one shot 
// at the same root level.
//
// Computing tasks do not return a value: they accumulate partial 
// results in a Reduction<double> object, and terminate
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

   // Construct a task list:
   // ---------------------
   task_list TL;
   for(int n=1; n<=Ntasks; ++n)
      {
      AreaTask *T = new(task::allocate_root()) AreaTask(0, 1, n);
      TL.push_back(*T);
      }
   task::spawn_root_and_wait(TL);
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
 


// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// AreaRec3.C
// ----------
// This example develops a TBB derived task class that performs the 
// parallel calculation of the area below a function that receives 
// and returns a double.

// STRATEGY:
// ---------
// Binary tree organization (each task spawns two children)
// Parents do not block on children, they specify a continuation task
//
// Slightly modified version of AreaRec2.C This version uses scheduler 
// bypass: a task launching children specifies the next task to
// run, instead of letting the scheduler decide. This may improve
// performance because we avoid queuing and dequeuing one of the 
// child tasks.
//
// *******************************************************************
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
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
float G;   // granularity
int   Nth;

// -----------------------------------------------------
// The continuation task class.
// ...........................
// Two child tasks will store in x and y the partial
// results of the two subranges
// This task puts them together, and returns the result
// to the parent task
// ----------------------------------------------------
class AreaContinuationTask : public task
   {
   public:
   double *sum;
   double x, y;

   AreaContinuationTask(double *sum_) : sum(sum_) {}
   task* execute()
      {
      *sum = x+y;
      return NULL;
      }
   };


// The main task class
// -------------------
class AreaTask : public task
   {
   public:
   double a, b;     // limits
   double *sum;     // return value

   AreaTask(double a_, double b_, double *sum_):
           a(a_), b(b_), sum(sum_) {}

   task *execute()
      {
      // Before we start, produce information about this task
      // ---------------------------------------------------------
      std::cout << "\nExecuting thread : " << pthread_self()
                << "\n Task (" << a << ", " << b  << ")"  
                << std::endl;
      // ---------------------------------------------------------
   
      if(fabs(b-a)<G)
         {
         *sum = Area(a, b, FCT);
         return NULL;
         }
      else
         {
         AreaContinuationTask &C = *new(allocate_continuation())
                                        AreaContinuationTask(sum);      
         double midval = (b-a)/2;
         AreaTask &A = *new(C.allocate_child()) 
                            AreaTask(a, a+midval, &C.x);
         AreaTask &B = *new(C.allocate_child()) 
                            AreaTask(a+midval, b, &C.y);
         C.set_ref_count(2);
         C.spawn(B);
         return &A;
         }
      }
   }; 


int main(int argc, char **argv)
   {
   double area;
   InputData();     // read Nth and G
   task_scheduler_init init(Nth);
   AreaTask& A = *new(task::allocate_root()) AreaTask(0, 1, &area);
   task::spawn_root_and_wait(A);
   std::cout << "\nArea is : " << area << std::endl;
   }

// Auxiliary I/O function
// ----------------------
void InputData()
    {
    FILE *fp;
    char buffer[128];

    if( !(fp = fopen("arearec.dat", "r") ))
	   {
	   std::cout << "\n Input error" << std::endl;
	   exit(0);
	   }
	
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &Nth);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%f", &G);
    fclose(fp);
    }
 




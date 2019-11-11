// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// AreaRec1.C
// ----------
// This example develops a TBB derived task class that performs the 
// parallel calculation of the area below a function that receives 
// and returns a double.
//
// STRATEGY:
// 
// Dynamic domain decomposition, as in the TBB algorithms. Binary tree 
// organization (each task spawns two children wgich compute the area of
// half of the domain, if the domain soze is above a given granularity)
// Parents block on children (blocking style with children), waiting for
// the return value of the area of the sub-domain
// *****************************************************************
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "tbb/task_scheduler_init.h"
#include "tbb/tbb_stddef.h"
#include "tbb/task.h"

using namespace tbb;

void InputData();       // forward declaration

// Library function to compute the area under func(x) in [a,b]
// -----------------------------------------------------------
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

double FCT(double x)            // function to integrate
   {
   return sin(x);
   }

// Global variables
// ---------------- 
float  G;   // granularity
int    Nth;


// The task class
// --------------
class AreaTask : public task
   {
   public:
   double a, b;     // limits
   double *sum;     // return value stored here

   AreaTask(double a_, double b_, double *sum_):
           a(a_), b(b_), sum(sum_) 
           {
           if(b<a) std::swap(a, b);
           }

   task *execute()
      {
      // Before we start, produce information about this task
      // ---------------------------------------------------------
      std::cout << "\nExecuting thread : " << pthread_self()
                << "\n Task (" << a << ", " << b  << ")" << std::endl;
      // ---------------------------------------------------------

      if(fabs(b-a)<G) *sum = Area(a, b, FCT);
      else
         {
         double x, y;
         double midval = (b-a)/2;
         AreaTask &A = *new(allocate_child()) 
                            AreaTask(a, a+midval, &x);
         AreaTask &B = *new(allocate_child()) 
                            AreaTask(a+midval, b, &y);

         // This task waits for two children: its refcount is 2+1
         set_ref_count(3);
         spawn(B);
         spawn_and_wait_for_all(A);
         *sum = x+y;
         }
      return NULL;
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
 




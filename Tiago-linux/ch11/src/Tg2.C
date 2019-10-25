// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File Tg2.C
//
// Second example of TBB taskgroup class.
//
// This is the recursive computation, with return values 
// to parent task. We exploit lambda expressions.
// -------------------------------------------------

#include <iostream>
#include <stdlib.h>
#include <math.h>
#include "tbb/task_scheduler_init.h"
#include "tbb/tbb_stddef.h"
#include "tbb/task_group.h"

#define EPSILON  0.0000001

using namespace tbb;

int    nTk;
double G;

// ---------------------------------------
// This is the definition of the function 
// to be integrated
// --------------------------------------
double my_fct(double a)
   {
   double retval;
   retval = 4.0 / (1.0+a*a); 
   return retval;
   }

// -------------------------------------------------
// This is a generic, sequential integration routine
// Integrates func(x) in [a, b] with precision eps
// ------------------------------------------------*/
double Area(double a, double b, double (*func)(double), 
            double eps=EPSILON)
   {
   int n, i, j;                        // internal usage 
   double s, snew, x, tnm, sum, del;   // internal usage 

   n = 1;
   i = 1; 
   snew = 0.5*(b-a)*(func(a)+func(b));
   do
      {
      s = snew;
      i <<=1;         // equivalent to i = (i<<1);
      tnm = i;
      del = (b-a)/tnm;
      x = a+0.5*del;
      for(sum=0.0, j=1; j<=i; j++, x+=del) sum += func(x);
      snew = 0.5*(s+(b-a)*sum/tnm);
      }while(fabs(s-snew)>EPSILON);
   return snew;
   }

// 
// Here we have an ORDINARY function that defines the
// task functions. This is a recursive function, not
// a class. Lambda expressions are used to transform
// this ordinary function into a function object when
// needed.
//
// In this example, every task that spawns two childs
// defines its own, local taskgroup object with the
// only object of waiting for the childs 
// ------------------------------------------------
double AreaTask(double a, double b)
   {
   double result;
   if(fabs(b-a)<G)
      result = Area(a, b, my_fct);
   else
      {
      double x, y;
      double midval = a+ 0.5*(b-a);
      task_group tg;

      // Function objects must be passed to run(). The lambda
      // expressions transform AreaTask into a function object
      // -----------------------------------------------------
      tg.run([&]{x = AreaTask(a, midval);});
      tg.run([&]{y = AreaTask(midval, b);});
      tg.wait();
      result = x+y;
      }
   return result;
   }

// --------------------
// The main function
// -------------------*/
int main (int argc, char *argv[])
   {
   int n, nTh;

   // --------------------------------------------------
   // Get number of threads and number of tasks from the
   // command line.
   // --------------------------------------------------

   // default values
   // --------------
   nTh = 2;
   nTk = 4;
   G = 0.4444;

   // override from command line
   // --------------------------
   if(argc==2) nTh = atof(argv[1]);
   if(argc==3) 
       {
       nTh = atof(argv[1]);
       nTk = atof(argv[2]);
       }

   // Set the number of threads
   // -------------------------
   task_scheduler_init init(nTh);
   double result = AreaTask(0, 1);
   std::cout << "\n Area result is : " << result << std::endl;
   }

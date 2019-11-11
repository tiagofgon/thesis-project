// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File Tg3.C
//
// Third example of TBB taskgroup class.
//
// Recursive computation, parent task do not wait for
// children: they spawn children and return. The final
// tasks that compute accumulate partial results in a
// mutex protected global variable
//
// Notice that in this example, ONE GLOBAL taskgroup
// object is required to wait for all descendants 
// -------------------------------------------------

#include <iostream>
#include <stdlib.h>
#include <math.h>
#include "tbb/task_scheduler_init.h"
#include "tbb/tbb_stddef.h"
#include "tbb/task_group.h"
#include "tbb/mutex.h"

#define EPSILON  0.0000001

using namespace tbb;

int    nTk;
double G;
task_group tg;    // global task_group

mutex  m;         // protects result
double result;    // cumulates partial results

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

// Here we have the function object that defines the
// task functions. This is a recursive function, not
// a class
// ------------------------------------------------
void AreaTask(double a, double b)
   {
   double res;
   if(fabs(b-a)<G)     // compute
      {
      res = Area(a, b, my_fct);
         {
         mutex::scoped_lock lock(m);
         result += res;
         }
      }
   else               // spawn childs and return
      {
      std::cout << "\n Splitting interval [" << a << "," << b 
                << "] " << std::endl;
      double midval = a + 0.5*(b-a);
      tg.run([=]{AreaTask(a, midval);});
      tg.run([=]{AreaTask(midval, b);});
      }
   }

// --------------------
// The main function
// -------------------*/
int main (int argc, char *argv[])
   {
   int n, nTh;
   double res;

   // --------------------------------------------------
   // Get number of threads and number of tasks from the
   // command line.
   // --------------------------------------------------

   // default values
   // --------------
   nTh = 2;
   nTk = 4;
   G = 0.2;

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

   tg.run([=]{AreaTask(0, 1);});
   tg.wait();
   std::cout << "\n Area result is : " << result << std::endl;
   }

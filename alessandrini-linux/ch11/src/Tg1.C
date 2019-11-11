// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File Tg1.C
//
// First example of TBB taskgroup class.
//
// Ntk tasks are launched, to compute the area under 
// a curve. The integration domain is split in Ntk identical
// segments, and each task computes a partial area result.
// Partial results are accumulated in a mutex protected global 
// variable.
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

double result;    // accumulates partial results
mutex  m;         // protects "result"
int    nTk;

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

// -------------------------------------------------
// Here we have the function object that defines the
// task functions for the TBB taskgroup class
// ------------------------------------------------
class AreaTask
   {
   private:
    int rank;

   public:
    AreaTask (int n) : rank(n) {}
   
    void operator() ()
       {
       double a, b, res;
       a = rank*(1.0/nTk);
       b = (rank+1)*(1.0/nTk); 
       res = Area(a, b, my_fct, EPSILON);
          {
          mutex::scoped_lock slock(m);
          result += res;
          }
       }
   };

// --------------------
// The main function
// -------------------*/
int main (int argc, char *argv[])
   {
   int n, nTh;
   double A = 0.0;
   double B = 1.0;

   // --------------------------------------------------
   // Get number of threads and number of tasks from the
   // command line.
   // --------------------------------------------------

   // default values
   // --------------
   nTh = 2;
   nTk = 4;
   result = 0.0;

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

   task_group tg;
   for(n=0; n<nTk; ++n) tg.run(AreaTask(n));
   tg.wait();
   std::cout << "\n Area result is : " << result << std::endl;
   }

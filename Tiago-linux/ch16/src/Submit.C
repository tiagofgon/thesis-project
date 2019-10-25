// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// Submit.C
//
// This example shows how to submit and wait for the termination
// of three successive parallel jobs, using the generic submission
// interface described in the book.
//
// Three complex jobs taken from previous TBB examples are chosen,
// to underline the flexibility of these programming interfaces.
// The complex jobs are the area computations using the task_group
// interface, described in chapter 11
//
// According to the procedure described in the book, complex jobs
// are encapsulated in a "work holder" class that represents the
// job submitted by main(). After submission, main() can do other
// things before waiting for the jobs.
//
// In the listing that follows, the three jobs and their work holder
// classes are defined first. Then, the different job submissions
// are performed in the function main().
// ***********************************************************
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <Timer.h>
#include <iostream>
#include "tbb/task_scheduler_init.h"
#include "tbb/tbb_stddef.h"
#include "tbb/task.h"
#include "tbb/mutex.h"
#include "tbb/task_group.h"

using namespace tbb;

// Globals
// -------
double result;
mutex m;
const double EPSILON=0.0000001;
int Nth, Ntk;
double G;
task_group tglobal;     // global task_group

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


// ***************************************************
// First job: area computation, fixed number of tasks,
// partial results collected in global variable,
// task_group used to run the parallel tasks
// ***************************************************
class AreaTask     // submitting Ntk tasks
   {
   private:
    int rank;

   public:
    AreaTask (int n) : rank(n) {}
   
    void operator() ()
       {
       double a, b, res;
       a = rank*(1.0/Ntk);
       b = (rank+1)*(1.0/Ntk); 
       res = Area(a, b, my_fct, EPSILON);
          {
          mutex::scoped_lock slock(m);
          result += res;
          }
       }
   };

// ------------------------------------------------
// The work holder WH1 class, that encapsulates the
// corresponding job to be done.
// -------------------------------------------
class WH1 : public task
   {
   public:
   task *execute()
      {
      int n;
      task_group tg;
      for(n=0; n<Ntk; ++n) tg.run(AreaTask(n));
      tg.wait();
      }
   }; 


// ***************************************************
// Second job: recursive area computation, tasks wait 
// for return values from children, task_group used to
// wait for children
// ***************************************************

// Task function for recursive computation, waiting
// for children. This is a recursive function, not
// a class
// ------------------------------------------------
double AreaRec1(double a, double b)
   {
   double result;
   if(fabs(b-a)<G)
      result = Area(a, b, my_fct);
   else
      {
      double x, y;
      double midval = a+ 0.5*(b-a);
      task_group tg;
      tg.run([&]{x = AreaRec1(a, midval);});
      tg.run([&]{y = AreaRec1(midval, b);});
      tg.wait();
      result = x+y;
      }
   return result;
   }

// The work holder class
// ---------------------
class WH2 : public task
   {
   public:
   task *execute()
      {
      int n;
      double x;
      task_group tg;
      tg.run([&]{x=AreaRec1(0, 1);});
      tg.wait();
      result = x;
      }
   }; 


// ***************************************************
// Third job: recursive area computation, tasks spawn 
// children and terminate, partial results collected in
// global variable, task_group used to run the tasks
// ***************************************************

// This is a recursive function, not a class
// -----------------------------------------
void AreaRec2(double a, double b)
   {
   double res;
   if(fabs(b-a)<G)
      {
      res = Area(a, b, my_fct);
         {
         mutex::scoped_lock lock(m);
         result += res;
         }
      }
   else
      {
      std::cout << "\n Splitting interval [" << a << "," << b 
                << "] " << std::endl;
      double midval = a + 0.5*(b-a);
      tglobal.run([=]{AreaRec2(a, midval);});
      tglobal.run([=]{AreaRec2(midval, b);});
      }
   }

// Work holder class
// -----------------
class WH3 : public task
   {
   public:
   task *execute()
      {
      int n;
      tglobal.run([=]{AreaRec2(0, 1);});
      tglobal.wait();
      }
   }; 

// ***************************************************
// This is an auxiliary template function to submit
// the parallel job. Template argument is the work 
// holder task class that encapsulates the job.
// ***************************************************
template <typename T>
empty_task* SubmitJob()
   {
   empty_task* E = new( task::allocate_root() ) empty_task;
   E->set_ref_count(2);
   T *t = new( E->allocate_child() ) T();
   E->spawn(*t);
   return E;
   }

// --------------------------------------------
// Auxiliary function to wait for submitted job
// --------------------------------------------
void WaitForJob(empty_task* E)
   {
   E->wait_for_all();
   E->destroy(*E);
   }


int main(int argc, char **argv)
   {
   Timer T;

   Nth = 2;
   Ntk = 4;
   result = 0.0;
   G = 0.28;
 
   // override from command line
   // --------------------------
   if(argc==2) Nth = atof(argv[1]);
   if(argc==3) 
       {
       Nth = atof(argv[1]);
       Ntk = atof(argv[2]);
       }

   task_scheduler_init init(Nth);
   std::cout << "\nMain starts "<< std::endl;

   // Launch Job
   // ----------
   result = 0.0;
   empty_task *e1 = SubmitJob<WH1>();

   // Do something else
   // -----------------
   std::cout << "\n First job submitted. Main waits for 5 seconds "
             << std::endl;
   T.Wait(5000);

   // Wait for first job 
   // ------------------
   WaitForJob(e1);    // e1 is destroyed here
   std::cout << "\nFirst job done. result is " << result <<  std::endl;

   // Launch second job
   // -----------------
   result = 0.0;
   empty_task *e2 = SubmitJob<WH2>();
   WaitForJob(e2);    // e2 is destroyed here
   std::cout << "\nSecond job done. result is " << result <<  std::endl;

   // Launch third job
   // -----------------
   result = 0.0;
   empty_task *e3 = SubmitJob<WH3>();
   WaitForJob(e3);    // e3 is destroyed here
   std::cout << "\nThird job done. result is " << result <<  std::endl;
   }





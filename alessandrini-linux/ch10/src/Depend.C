// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File Depend.C
//
// Using the recursive area calculation of PI, this example 
// illustrates the usage of the "depend" clause in the "task" 
// directive. It used to build up a barrier among TASKS, not 
// threads.
//
// Ntk+1 tasks are launched. Nth tasks compute in parallel the 
// area under a curve. The remaining task prints the result of 
// the calculation. A barrier must therefore be established among 
// the computing tasks, before the last task is executed and the
// result is printed.
// -------------------------------------------------

#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define EPSILON  0.0000001

using namespace std;

double result;    // global variable
int    synch;     // used to synchronize tasks

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
double Area(double a, double b, double (*func)(double), double eps=EPSILON)
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

// --------------------
// The main function
// -------------------*/
int main (int argc, char *argv[])
   {
   int nTh, nTk;
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
   synch = 0;

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
   omp_set_num_threads(nTh);

   // ----------------------------------------------
   // Create the parallel region, but make sure that
   // the initial implicit task is only executed by
   // a single worker thread  
   // ---------------------------------------------

   #pragma omp parallel
      {
      #pragma omp single
         {
         int n;
         // spawn the nTk computing tasks
         // -----------------------------
         for(n=0; n<nTk; ++n)
            {
            #pragma omp task depend(in: synch)
               {
               int rank = n;
               double a, b, res;
               a = rank*(B-A)/nTk;
               b = (rank+1)*(B-A)/nTk; 
               res = Area(a, b, my_fct, EPSILON);
               #pragma omp critical
                  {
                  cout << "\n Partial result task " << rank << " : "
                       << res << endl;
                  result += res;
                  }
               synch++;
               }
            }

         // Next, spawn the printing task. Since this task has an
         // "out" dependency on synch, it is executed fter the 
         // previous tasks that have an "in" dependency on synch.
         // The book has an in-depth discussion of this example
         // -----------------------------
         #pragma omp task depend(out: synch)
            {
            synch++;
            cout << "\n Area result from inner task is " << result; 
            cout << "\n Value of synch " << synch << endl;
            
            }
         #pragma omp taskwait
         }
      } 
   cout << "\n Area result is " << result << endl;
   return 0;
   }

// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File AreaPiOmp.C
//
// OpenMP version of the recursive area calculation of PI. 
// A recursive domain decomposition is implemented: 
// 
// A task that gets a domain size bigger than a given granularity
// splits the domain, lauhches two child tasks for each subdomain, 
// and waits for them to return the partial area result.
//
// A task that finally gets a domain size smaller or equal to the
// granularity, computes the area and returns the result.
//
// Several issues are underlined in this example:
// ---------------------------------------------
// - The usage of the "task" directive.
// - The way tasks capture parent data by reference, using the
//   "shared" clause, so that they can return a value to the
//   parent task.
// -----------------------------------------------------------

#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define EPSILON  0.0000001

using namespace std;

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

// ---------------------------------------------------
// This is a generic but recursive integration routine
// Splits the integration domain in two halves as long
// as the domain is bigger than L. When the domain
// decomposition is finished, it calls the sequential
// routine
// ---------------------------------------------------
double AreaRec(double a, double b, double L, 
            double (*func)(double), double eps=EPSILON)
   {
   double x, y, medval, retval;
   int nTs, thN;
   
   nTs = omp_get_num_threads();
   thN = omp_get_thread_num();

   // ID message
   cout << "\n AreaRec task call  (" << a << ",  " << b << ")  L=" << L;
   cout << "\n Executed by thread " << thN << " with " << nTs 
        << " threads active\n" << endl;

   if( fabs(b-a) > L)
      {
      medval = 0.5*(b-a);

      #pragma omp task untied shared(x)   // x captured by address 
      x = AreaRec( a, a+medval, L, func);
      #pragma omp task untied shared(y)   // y captured by address
      y = AreaRec( a+medval, b, L, func);
      #pragma omp taskwait

      retval = x+y;
      }
   else retval = Area(a, b, func, eps);
   return retval;
   }


// --------------------
// The main function
// -------------------*/
 
int main (int argc, char *argv[])
   {
   int n, nTh;
   double G, result;
   double A = 0.0;
   double B = 1.0;
   G = (B-A)/4;

   // --------------------------------------------------
   // Get the number of threads from the command line
   // default is 4 threads
   // --------------------------------------------------

   if(argc==2) nTh = atof(argv[1]);
   else nTh = 4;

   // Set the number of threads
   // -------------------------
   omp_set_num_threads(nTh);

   // ----------------------------------------------
   // Create the parallel region, but make sure that
   // the initial implicit task is only executed by
   // the one thread in the team 
   // ---------------------------------------------
   #pragma omp parallel
      {
      #pragma omp single
         {
         result = AreaRec(A, B, G, my_fct);
         }
      } 

   cout << "\n result = " << result << endl;
   return 0;
   }

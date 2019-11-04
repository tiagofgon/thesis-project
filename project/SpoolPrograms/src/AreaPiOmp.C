// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// **************************************
// File AreaPiOmp.C - OpenMP code
// 
// Computation of the area under a function f(x) 
// in the interval [a, b].
// The area under f(x) = 4.0/( 1+x*x) in [0, 1] 
// is PI.
// ---------------------------------------------

#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <omp.h>

using namespace std;

// ----------------------------------------------------
// Generic, sequential library integration routine,
// integrates func(x) in [a, b] with a given precision. 
// Taken from NRC.
// ----------------------------------------------------

double Area(double a, double b, double (*func)(double), 
            double precision)
   {
   int n, i, j;                      /* internal usage */
   double s, snew, x, tnm, sum, del; /* internal usage */

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
      }while(fabs(s-snew)>precision);
   return snew;
   }

// ------------------------------------
// Define the function to be integrated
// ------------------------------------
double my_fct(double x)
   {
   double retval;
   retval = 4.0 / (1.0 + x*x);
   return retval;
   }

// -------------
// Global data
// ------------

double  *result;   // this array contains partial results
int     nTh;       // number of threads

// -----------------
// Thread function
// -----------------
void thread_fct()
   {
   double a, b; 
   double retval, length;
   int rank = omp_get_thread_num();

   // Compute the limits of integration for this thread.
   // Notice: in OpenMP, thread ranks start from 0, not 1
   // ---------------------------------------------------
   length = 1.0/nTh;
   a = rank * length;
   b = (rank+1) * length;

   // Compute the partial area, and store result
   // ------------------------------------------
   retval = Area(a, b, my_fct, 0.0000001);
   result[rank] = retval;
   }

// -----------------
// The main funcion
// -----------------
int main(int argc, char **argv)
   {
   double pi;
  
   // Set the number of threads
   // ------------------------- 
   if(argc==2) nTh = atoi(argv[1]);
   else nTh = 4; 
   
   // Allocate result[] array. Notice: in OpenMP,
   // we start storing from 0
   // ------------------------------------------
   result = new double[nTh];

   // Run threads
   // -----------
   omp_set_num_threads(nTh);
   #pragma omp parallel
      {
      thread_fct();
      }
 
   // Cumulate partial results
   // ------------------------
   pi = 0.0;
   for(int n=0; n<nTh; ++n) pi += result[n];

   cout << "\n Value of PI = " << pi << endl;
   delete [] result;
   }

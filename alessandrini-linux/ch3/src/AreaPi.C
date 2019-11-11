// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File AreaPi.C
// ---------------------------------------------------
// Computation of the area under a function f(x) in the
// interval [a, b].
//
// The area under f(x) = 4.0/( 1+x*x) in [0, 1] is PI.
// --------------------------------------------------

#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <SPool.h>

using namespace std;

// ----------------------------------------------------
// Generic, sequential library integration routine,
// integrates func(x) in [a, b] with a given precision. 
// This routine is taken from from NRC.
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
SPool   *TH;       // reference to SPool
double  *result;   // this array stored partial results
int     nTh;       // number of threads

// -----------------
// Thread function
// -----------------
void thread_fct(void *P)
   {
   double a, b; 
   double retval, length;
   int rank = TH->GetRank();

   // Split the integration domain in nTh idential subdomains,
   // and assign to this thread a subdomain according to its
   // rank
   // -------------------------------------------------------
   length = 1.0/nTh;
   a = (rank-1) * length;
   b = rank * length;

   // Compute the partial area, and store the partial result
   // in the global result[] array
   // ------------------------------------------------------
   retval = Area(a, b, my_fct, 0.0000001);
   result[rank] = retval;
   }

// -----------------
// The main funcion
// -----------------
int main(int argc, char **argv)
   {
   double pi;
  
   // Set the number of threads from command line
   // (default is 4 threads)
   // ------------------------------------------- 
   if(argc==2) nTh = atoi(argv[1]);
   else nTh = 4; 
   
   // Allocate result array and thread pool. Notice
   // that we add an extra slot to result[] because
   // we intend to store starting from 1
   // ---------------------------------------------
   result = new double[nTh+1];
   TH = new SPool(nTh);

   // Run threads
   // -----------
   TH->Dispatch(thread_fct, NULL);
   TH->WaitForIdle();
 
   // Cumulate partial results
   // ------------------------
   pi = 0.0;
   for(int n=1; n<=nTh; ++n) pi += result[n];

   cout << "\n Value of PI = " << pi << endl;
   delete TH;
   delete [] result;
   }

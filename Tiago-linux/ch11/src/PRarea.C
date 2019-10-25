// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File PRarea.C
// 
// This example uses the parallel_reduce algorithm in a
// problem which is not the parallel computation of a
// loop: the recursive computation of the area under a 
// curve.
//
// A RealRange class is defined to describe a splittable
// segment on the real axis, to be able to perform a recursive
// domain decomposition. Look at file RealRange.h
// ===========================================================
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <InputList.h>
#include <RealRange.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/parallel_reduce.h>

using namespace tbb;

// -------------------------------------------------------------
// This is an auxiliary function that computes the area of "func" 
// in a given interval (a,b). This is an improved method that 
// stops after a precision of 0.001 is reached
// --------------------------------------------------------------
double area(double a, double b, double(*func)(double))
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


// -------------------------------------------------
// The Body class for the recursive area computation
// -------------------------------------------------
class AreaBody
   {
   public:
   double partial_area;
   double (*fct)(double x);

   // Constructors
   // ------------
   AreaBody(double (*F)(double)) : fct(F), partial_area(0.0) {}
   AreaBody(AreaBody& A, split) : fct(A.fct), partial_area(0.0) {}

   void operator() (const RealRange& R)
      {
      partial_area += area(R.begin(), R.end(), fct);
      // print message
      // -------------
      std::cout << "\nArea in interval ( " << R.begin() << ", " << R.end()
                << " ) is " << partial_area << std::endl;
      }
      
   void join(const AreaBody& A)
      {
      partial_area += A.partial_area;
      }
   };

// -------------------------
// Function to be integrated
// ------------------------- 
double FCT(double x)
   {
   return sin(x);
   }


int main(int argc, char **argv)
   {
   double a, b;
   int nTh, Gr;

   // ----------- input ----------------------
   InputList IL;
   IL.RegisterData("a", &a, NF, 1);
   IL.RegisterData("b", &b, NF, 1);
   IL.RegisterData("Gr", &Gr, NI, 1);
   IL.RegisterData("nTh", &nTh, NI, 1);
   IL.ReadData("prarea.dat");
   IL.PrintData();
   // -----------------------------------------

   a = 0.0;
   b = 3.1416;
   task_scheduler_init init(nTh);
   double granularity = (b-a)/Gr;
   std::cout << "\nGranularity is : " << granularity << std::endl;

   RealRange R(a, b, Gr);
   AreaBody B(FCT);
   parallel_reduce(R, B);
   std::cout << "\nFinal result for area is : " << B.partial_area
             << std::endl;
   return 0;
   }


//////////////////////////////////////////////////////////////////

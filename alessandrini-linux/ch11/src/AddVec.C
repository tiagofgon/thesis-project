// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File AddVector.C
// 
// This example uses the parallel_for TBB algorithm to compute
// the addition of two vectors X and Y: X += Y
//
// Input data is read from file "minval.dat":
// - N, the vector size
// - Gr, the granularity that limits the recursive domain 
//       decomposition
// - nTh, the number of threads
//
// By reducing the granularity, the number of parallel tasks is
// increased.
// ------------------------------------------------------------

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <IntRange.h>
#include <Rand.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/parallel_for.h>

using namespace tbb;

// ------------
// Global data
// -----------
int N;         // initialized from file "minval.dat"
int nTh, Gr;   // idem


// -------------------------------------------------------------
// Auxiliary function for input. It reads from file "minval.dat" 
// the vector size N, an integer Gr that determines granularity 
// and the number of threads in the pool.
// -------------------------------------------------------------
void InputData()
    {
    FILE *fp;
    char buffer[128];

    if( !(fp = fopen("minval.dat", "r") ))
	   {
	   std::cout << "\n Input error" << std::endl;
	   exit(0);
	   }
	
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &N);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &Gr);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &nTh);
    fclose(fp);
    }

// ---------------------------------------------
// The Body class for the map (x, y) -> (x+y, y)
// The Range class used in thos example is in file
// IntRange.h 
// ---------------------------------------------
class AddVectorBody
   {
   public:
   double *X, *Y;

   // Constructor
   // -----------
   AddVectorBody(double *x, double *y) : X(x), Y(y) {}

   void operator() (const IntRange& R) const
      {
      for(size_t n=R.begin(); n!=R.end(); ++n)
         X[n] += Y[n];
      }
   };


// -----------------
// The main function
// -----------------
int main(int argc, char **argv)
   {
   double *X, *Y;
   Rand Rd(999);

   InputData();

   X = new double[N];  // allocate and initialize
   Y = new double[N];
   for(int n=0; n<N; ++n)
      {
      X[n] = Rd.draw();
      Y[n] = Rd.draw();
      }

   std::cout << "\n X[0] = " << X[0] << ",  X[N/2] = " << X[N/2];
   std::cout << "\n Y[0] = " << Y[0] << ",  Y[N/2] = " << Y[N/2]
             << std::endl;

   task_scheduler_init init(nTh);
   IntRange R(0, N, Gr);
   AddVectorBody B(X, Y);
   parallel_for(R, B);

   std::cout << "\n X[0] = " << X[0] << ",  X[N/2] = " << X[N/2]
             << std::endl;
   return 0;
   }

//////////////////////////////////////////////////////////////////

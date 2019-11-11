// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File Foreach.C
//
// This example is designed to show the high efficiency of tasks
// in executing a VERY UNBALANCED map operation.
//
// A map operation modifies each element of a container, and the
// operation implemented here is very erratic.
// ------------------------------------------------------------

#include <iostream>
#include <stdlib.h>
#include <Rand.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <omp.h>
#include <CpuTimer.h>
#include <SafeCounter.h>

using namespace std;

// Global variables
// ----------------
int  nTh;
int  N;
double p_initial, p_final;
std::vector<double> V;
SafeCounter SC;

double precision(int n)
   {
   double a  = (p_final - p_initial)/N;
   return (a*n+p_initial);
   }

void Replace(int n) 
   { 
   Rand R(999 * SC.Next());
   double x;
   double eps = precision(n);
   double d = V[n];
   do
      {
      x = R.draw();
      }while( fabs(x-d)>eps );
   V[n] = x;
   }


// The main function
// -----------------

int main(int argc, char **argv)
   {
   int n;
   int scale;
   CpuTimer T;
   Rand rd(777);

   scale = 1000;
   p_initial = 0.000001;
   if(argc==2) p_initial = atof(argv[1]);
   if(argc==3)
      {
      p_initial = atof(argv[1]);
      scale = atoi(argv[2]);
      }
   p_final = p_initial * scale;
   N = 1000000;
   nTh = 4;

   // Initialization of vector target
   // -------------------------------
   for(int n=0; n<N; ++n) V.push_back(rd.draw() );

   // First, sequential computation
   // -----------------------------
   T.Start();
   for(n=0; n<N; ++n) Replace(n);
   T.Stop();
   T.Report();
   cout << "\nAfter sequential : vector components 0, N/2, (N-1):\n"
        << V[0] << "       " << V[N/2] << "       " << V[N-1] 
        << "\n --------------------------------------------" << endl;

   omp_set_nested(1);
   omp_set_num_threads(4);

   // Next, parallel for computation, using parallel for
   // with chunks of size N/48
   // ---------------------------------------------------
   T.Start();
   #pragma omp parallel for schedule(guided, N/48)
     for(n=0; n<N; ++n) Replace(n);
   T.Stop();
   cout << "\n\n parallel for computation: ";
   T.Report();

   // Next, task computation (the best!)
   // -----------------------------
   T.Start();
   #pragma omp parallel
      {
      #pragma omp single
         {
         n = 0;
         while(n < N)
            {
            #pragma omp task untied
              { Replace(n); }
            n++;
            }
         }
      #pragma omp barrier
      }
   T.Stop();
   cout << "\n ------------------------------------\n";
   cout << "\n parallel task computation: ";
   T.Report();
   cout << "\nAfter task : vector components 0, N/2, (N-1):\n"
        << V[0] << "       " << V[N/2] << "       " << V[N-1] << endl;
   }

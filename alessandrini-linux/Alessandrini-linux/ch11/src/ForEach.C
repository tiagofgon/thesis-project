// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File Foreach.C
// 
// Map operation in a containern with
// TBB threads, using parallel_for_each.
//
// In this example, we run the problem twice:
// first, with sequential STL for_each, next
// with TBB parallel_for_each.
// -----------------------------------

#include <iostream>
#include <stdlib.h>
#include <Rand.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <tbb/task_scheduler_init.h>
#include <tbb/parallel_for_each.h>
#include <tbb/tick_count.h>
#include <SafeCounter.h>

using namespace tbb;
using namespace std;

// Global variables
// ----------------
int  nTh;
int  N;
std::vector<double> V;
SafeCounter SC;

class Replace 
   { 
   private:
     double precision;

   public:
    Replace(double d) : precision(d) {}

    void operator()(double& d) const
       {
       Rand R(999 * SC.Next());
       double x;
       do
          {
          x = R.draw();
          }while( fabs(x-d)>precision );
       d = x;
       }
   };


// The main function
// -----------------

int main(int argc, char **argv)
   {
   Rand rd(777);
   double precision;

   if(argc==2) precision = atof(argv[1]);
   else precision = 0.001;
   N = 10000000;
   nTh = 2;

   // Initialization of vector target
   // -------------------------------
   for(int n=0; n<N; ++n) V.push_back(rd.draw() );

   // First, sequential computation
   // -----------------------------
   tick_count t0 = tick_count::now();
   Replace F(precision);
   for_each(V.begin(), V.end(), F);
   tick_count t1 = tick_count::now();
   cout << "\nVector components 0, N/2, (N-1):"
        << V[0] << "       " << V[N/2] << "       " << V[N-1] << endl;
   std::cout << "\n Wall time = " << (t1-t0).seconds() << " seconds"
             << std::endl;

   // Next, parallel computation
   // -----------------------------
   task_scheduler_init init(nTh);
   tick_count t2 = tick_count::now();
   Replace G(precision);
   parallel_for_each(V.begin(), V.end(), G);
   tick_count t3 = tick_count::now();
   cout << "\nVector components 0, N/2, (N-1):"
        << V[0] << "       " << V[N/2] << "       " << V[N-1] << endl;
   std::cout << "\n Wall time = " << (t2-t3).seconds() << " seconds"
             << std::endl;
   }

// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File MinVal.C
// 
// This eample is a slightly modified version of an example
// proposed in the TBB documentation: scanning in parallel
// a vector to search for the minimum vector element as well
// as its position (vector index).
//
// The modifications made are simple: we use a vector of doubles
// (instead a vector of integers) - this modification is irrelevant -
// and we have added some extra debug information in the code to
// check in detail how the parallel_reduce algorithm operates
// -----------------------------------------------------------------
 
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <IntRange.h>
#include <SafeCounter.h>
#include <Rand.h>
#include <pthread.h>
#include <tbb/task_scheduler_init.h>
#include <tbb/parallel_reduce.h>
#include <tbb/mutex.h>

using namespace tbb;

// ---------------------------------------------
// Global data, initialized from PRarea.dat file
// ---------------------------------------------
int N;          // vector size
int nTh, Gr;    // nb of threads, granumarity
double *V;      // target vector
mutex M;        // protects stdout 

// The SafeCounter class is used to retrieve - in a thread safe
// way - consecutive integer values used to identify objects
// ------------------------------------------------------------
SafeCounter C;

// -------------------------------------------------------------
// Auxiliary function for input. It reads from file "minval.dat" 
// the vector size, an integer Gr that determines granularity as 
// (b-a)/Gr, and the number of threads
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
// The Body class for the parallel search of the 
// minimum value
// ---------------------------------------------
class MinvalBody
   {
   public:
   double minval;
   int    minindex;
   int    body_id;   // this is added to identify the body objects
                     // created by the split constructor when 
                     // a task is stolen.
   double *V;

   // Constructors
   // ------------
   MinvalBody(double *A) : 
      V(A), 
      minval(100000.0), 
      minindex(-1), body_id(C.Next()) {}

   MinvalBody(MinvalBody& MV, split) : 
      V(MV.V), 
      minval(100000.0),
      minindex(-1), body_id(C.Next())  {}

   void operator() (const IntRange& R)
      {
      for(size_t n=R.begin()+1; n!=R.end(); ++n)
         {
         double value = V[n];
         if( value < minval ) 
            {
            minval = value;
            minindex = n;
            }
         }
         
      // before exiting, print ID message with locked mutex
      // This is the debug info.
      // --------------------------------------------------
      mutex::scoped_lock lock;
      lock.acquire(M);
      std::cout << "\n Body ID : " << body_id << ",  thread = " 
                << pthread_self();
      std::cout << "\nInterval ( " << R.begin() << ", " 
                << R.end() << " ), minval = " << minval 
                   << ", at index " << minindex << std::endl;
      std::cout << std::endl;
      lock.release();
      }
      
   void join(const MinvalBody& A)
      {
      if(A.minval < minval) 
         {
         minval = A.minval;
         minindex = A.minindex;
         }
      }
   };

// -----------------
// The main function
// -----------------
int main(int argc, char **argv)
   {
   Rand Rd(999);
   InputData();
   std::cout << "\n N   = " << N
             << "\n Gr  = " << Gr
             << "\n nTh = " << nTh << std::endl;
   std::cout << "\nGranularity is : " << N/Gr << std::endl;

   V = new double[N];  // allocate and initialize
   for(int n=0; n<N; ++n) V[n] = 200.0 * Rd.draw() - 100.0;

   task_scheduler_init init(nTh);
   IntRange R(0, N, Gr);
   MinvalBody B(V);
   parallel_reduce(R, B);
   std::cout << "\nOverall minimum value is : " << B.minval
             << "  at index " << B.minindex << std::endl;
   return 0;
   }


//////////////////////////////////////////////////////////////////

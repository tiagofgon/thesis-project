// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File Foreach.C
//
// This is an unbalanced map operation on a vector
// container of doubles.
//
// Each vector element is initialized with a random
// number in [0, 1].
//
// The map operation: task replaces the vector element
// with a very close value - within a given precision -
// obtained by calling a privata random generator. The
// amount of work on each element is random. Moreover,
// the precision decreases as we move along the container,
// so that the average work also decreases. This makes
// the whole operation unbalanced (otherwise, it would be
// balanced on the average).
//
// The map operation is performed in two ways
// *) Sequential computation, for comparison
// *) Parallel computation, launching one task per
//    vector element. A nice speedup is observed
//
//  Notice how the Safecounter class is used to 
//  initialize the local random number generators.
// ------------------------------------------------

#include <iostream>
#include <stdlib.h>
#include <Rand.h>
#include <math.h>
#include <vector>
#include <algorithm>
#include <NPool.h>
#include <CpuTimer.h>
#include <SafeCounter.h>

using namespace std;

// Global variables
// ----------------
int  nTh;
int  N;
double p_initial, p_final;  // initial and final precisions
std::vector<double> V;
SafeCounter SC;
NPool       *TP;

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

// New: a class for the NPool computation
// --------------------------------------
class ReplaceTask : public Task
   {
   private:
    int n;

   public:
    ReplaceTask (int nn) : n(nn) {}

    void ExecuteTask()
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
   };

// The main function
// -----------------
int main(int argc, char **argv)
   {
   int n, jobID;
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

   // Initializations: vector target, pool
   // --------------------------------------
   for(int n=0; n<N; ++n) V.push_back(rd.draw() );
   TP = new NPool(nTh);

   // First, sequential computation
   // -----------------------------
   T.Start();
   for(n=0; n<N; ++n) Replace(n);
   T.Stop();
   T.Report();
   cout << "\nAfter sequential : vector components 0, N/2, (N-1):\n"
        << V[0] << "       " << V[N/2] << "       " << V[N-1] << endl;

   // Next, NPool computation
   // Construct first a huge TaskGroup encapsulating
   // all the tasks
   // ----------------------------------------------
   TaskGroup *TG = new TaskGroup();
   for(int k=0; k<N; k++)
      {
      ReplaceTask *t = new ReplaceTask(k);
      TG->Attach(t);
      }
 
   T.Start();
   // -----------------------------------------
   jobID = TP->SubmitJob(TG);   // submit
   TP->WaitForJob(jobID);       // wait for job
   // -----------------------------------------
   T.Stop();

   cout << "\n parallel task computation: " << endl;
   T.Report();
   cout << "\nAfter task : vector components 0, N/2, (N-1):\n"
        << V[0] << "       " << V[N/2] << "       " << V[N-1] << endl;
   }

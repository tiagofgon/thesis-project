// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File AreaThp.C
//
// Standard, non recursive parallel area computation. 
// given the interval  [a,b] and the number of tasks 
// in the parallel job
// ------------------------------------------------

#include <iostream>
#include <math.h>
#include <stdio.h>
#include <NPool.h>
#include <Reduction.h>

using namespace std;

#define EPSILON  0.000001

// Global data
// -----------
NPool   *NP;                   // thread pool
int     nTasks, Nth;           // number of tasks and threads
const double eps = 0.0000001;  // precision
Reduction<double> RD;          // to perform reduction

void InputData();

// Function to be integrated
// -------------------------
double FCT(double x)
   {
   double retval;
   retval = 4.0 / (1.0 + x*x);
   return retval;
   }


// ----------------------------------------
// Generic, sequential integration routine,
// integrates func(x) in [a, b] with a given
// precision
// --------------------------------------
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

// ------------------
// The task class
// -----------------
class AreaTask: public Task
   {
   private:
    int rank;

   public:
    AreaTask(int r): Task(), rank(r) {}

    void ExecuteTask()
       {
       double a, b, retval;
       a = (double)(rank-1)/nTasks;
       b = (double)rank / nTasks;
       retval = Area(a, b, FCT, eps);
       RD.Accumulate(retval);
       }
    };


// The main function
// *****************
int main (int argc, char *argv[])
   {
   int jobID;
   double result;

   InputData();      // get Nth and nTasks

   // Initialize the thread pool
   // --------------------------
   NP = new NPool(Nth, 20);

   // Construct Task Group, and submit
   // --------------------------------
   TaskGroup *TG = new TaskGroup();
   for(int n=1; n<=nTasks; ++n)
      {
      AreaTask *T = new AreaTask(n);
      TG->Attach(T);
      }
   jobID = NP->SubmitJob(TG);
   NP->WaitForJob(jobID);
   result = RD.Data(); 
   cout << "\n result = " <<  result << endl;
   return 0;
   }


// ---------------------------------------------------
// This function reads from file "area.dat" the number
// of threads Nth and the number of tasks nTasks
// Uses traditional C I/O. 
// --------------------------------------------------
void InputData()
    {
    FILE *fp;
    char buffer[128];

    if( !(fp = fopen("area.dat", "r") ))
	   {
	   cout << "\n Input error" << endl;
	   exit(0);
	   }
	
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &Nth);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &nTasks);
    fclose(fp);
    }


// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File AreaRec3.C
// 
// Recursive computation of area under a function, 
// given interval [a,b] and criterion to stop
// recurrence.
// We show here how to manage return values. There
// is no global reduction, and all tasks block on
// children. 
//
// This is the same as AreaRec2.C, but we have inserted some
// debug information tracking the execution of the different
// tasks..
// ------------------------------------------------

#include <iostream>
#include <sstream>
#include <math.h>
#include <stdio.h>
#include <NPool.h>
#include <SafeCout.h>

using namespace std;

// Global data
// -----------
NPool *NP;                    // thread pool
float  G;                     // granularity
int    Nth;                   // number of threads
const double eps = 0.000001;  // precision
SafeCout SC;

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

// ---------------------------------------------------
// The task class. Implements a generic, but recursive 
// integration routine
// Splits the integration domain in two halves as long
// as the domain is bigger than G. When the domain
// decomposition is finished, it calls the sequential
// routine for each subdomain.
// ---------------------------------------------------
class AreaTask: public Task
   {
   private:
    double a, b;
    double *sum;
    ostringstream os;

   public:
    AreaTask(double _a, double _b, double *s): Task(), 
                                               a(_a), b(_b), sum(s) {}
    void ExecuteTask()
       {
       double medval, retval;
       if( fabs(b-a) > G)
          {
          double x, y;
          medval = 0.5*(b-a);
          AreaTask *T1 = new AreaTask(a, a+medval, &x);
          AreaTask *T2 = new AreaTask(a+medval, b, &y);
          NP->SpawnTask(T1);
          NP->SpawnTask(T2);
          os << "\n Task with a = " << a << "  b = " << b 
               << " : Two tasks launched";
          SC.Flush(os);
          NP->TaskWait();
          retval = x+y;
          os << "\n Task with a = " << a << "  b = " << b 
               << " : Got return values";
          SC.Flush(os);
          }
       else retval = Area(a, b, FCT, eps);
       *sum = retval;
       os << "\n Task with a = " << a << "  b = " << b 
          << " : completed";
       SC.Flush(os);
       }
   };

class SubmitTask: public Task
   {
   private:
    double result;
    ostringstream os;

   public:
    SubmitTask() {}
    void ExecuteTask()
       {
       // Submit task, and wait for idle
       // -------------------------------
       AreaTask *T = new AreaTask(0, 1, &result);
       int jobID = NP->SubmitJob(T);
       NP->WaitForJob(jobID);
       cout << "\n Result is = " <<  result << endl;
       os << "\n Submit Task completed";
       SC.Flush(os);
       }
   };

// The main function
// *****************
int main (int argc, char *argv[])
   {
   int jobID;
   double result;

   InputData();    // read Nth and G from file
   std::cout << "\nValue of G = " << G << std::endl;

   // Initialize the thread pool
   // --------------------------
   NP = new NPool(Nth, 20);

   // Submit task, and wait for idle
   // -------------------------------
   AreaTask *T = new AreaTask(0, 1, &result);
   jobID = NP->SubmitJob(T);
   NP->WaitForJob(jobID);
   cout << "\n Main terminates, result = " << result << endl;
   return 0;
   }

// ---------------------------------------------------
// This function reads from file "area.dat" the number
// of threads Nth and the granularity G of the recursive
// process. 
// --------------------------------------------------
void InputData()
    {
    FILE *fp;
    char buffer[128];

    if( !(fp = fopen("arearec.dat", "r") ))
	   {
	   cout << "\n Input error" << endl;
	   exit(0);
	   }
	
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &Nth);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%f", &G);
    fclose(fp);
    }


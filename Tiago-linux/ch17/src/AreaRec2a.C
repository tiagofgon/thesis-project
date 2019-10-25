// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File AreaRec2a.C
// 
// Recursive computation of area under a function, 
// given interval [a,b] and criterion to stop
// recurrence.
// We show here how to manage return values. There
// is no global reduction, and all tasks block on
// children. .
// ------------------------------------------------

#include <iostream>
#include <sstream>
#include <math.h>
#include <NPool.h>
#include <SafeCout.h>

using namespace std;

// Global data
// -----------
NPool *NP;                    // thread pool
float  G;                     // granularity
int    Nth;                   // number of threads
const double eps = 0.000001;  // precision
SafeCout SC;                  // for sordered IO to stdout

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
          // -------------------------------------------
          os << "\n Task [" << a << ", " << b << "] :" 
               << " Two tasks launched";
          SC.Flush(os);
          // -------------------------------------------
          NP->TaskWait();
          retval = x+y;
          // -------------------------------------------
          os << "\n Task [" << a << ", " << b << "] :" 
               << " Got return values";
          SC.Flush(os);
          // -------------------------------------------
          }
       else retval = Area(a, b, FCT, eps);
       *sum = retval;
       // ------------------------------------------
       os << "\n Task [" << a << ", " << b << "] :" 
       << " completed, returned " << retval;
       SC.Flush(os);
       // ------------------------------------------
       }
   };


// The main function
// *****************
int main (int argc, char *argv[])
   {
   int n, jobID1, jobID2;
   double result1, result2;

   InputData();    // read Nth and G from file
   if(argc==2) Nth = atoi(argv[1]);

   std::cout << "\nValue of G = " << G << std::endl;

   // Initialize the thread pool
   // --------------------------
   NP = new NPool(Nth, 20);

   // Submit recursive single task job
   // --------------------------------
   AreaTask *T = new AreaTask(0, 1, &result1);
   jobID1 = NP->SubmitJob(T);
   
   // Submit again, single task job
   // -----------------------------
   AreaTask *Tbis = new AreaTask(0, 1, &result2);
   jobID2 = NP->SubmitJob(Tbis);

   // Check results:
   // -------------
   NP->WaitForJob(jobID1);
   cout << "\n First result is = " <<  result1 << endl;
   NP->WaitForJob(jobID2);
   cout << "\n Second result is = " <<  result2 << endl;

   // Delete pool
   // -----------
   delete NP;
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


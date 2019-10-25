// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File Recursive.C
//  
// Recursive generation of tasks. Each task generates two child tasks 
// with an increased deepness paramemer. Recursive generation stops 
// when the deepness parameter is D, read from command line (default 
// value is 3) 
// ------------------------------------------------------

#include <iostream>
#include <sstream>
#include <NPool.h>
#include <Timer.h>
#include <SafeCout.h>

// Global data
// -----------
NPool          *NP;        // thread pool
SafeCout        Scout;      // for ordered output to sodout
int             D;          // deepnes of recurrence

// -------------------------------
// Generic, but recursive routine
// Receives the address of nSamples
// -------------------------------
class RecTask: public Task
   {
   private:
    int deep;                 // deepness of this task
    int timewait;             // time duration
    std::ostringstream os;    // ordered outmut to stdout
    Timer T;

   public:
    RecTask(int d, int tw): Task(), deep(d), timewait(tw){}
    void ExecuteTask()
       {
       
       long count = 0;
       double x, y;

       if(deep<D)
          {
          RecTask *T1 = new RecTask(deep+1, timewait); 
          RecTask *T2 = new RecTask(deep+1, timewait);
          NP->SpawnTask(T1);
          NP->SpawnTask(T2);
          // ----------------------------------------------
          os << "\n Task with deepness " << deep 
             << " submitted two childs" ; Scout.Flush(os);
          // ----------------------------------------------
          }
      else
         {
         T.Wait(timewait);
         // -----------------------------------
         os << "\n Task with deepness " << deep 
            << " terminated"; Scout.Flush(os);
         // -----------------------------------
         }
      }
   //end of class  
   };


// The main function
// *****************
int main (int argc, char *argv[])
   {
   int Nth, jobID;

   // Get data from command line
   // --------------------------
   if(argc == 2) D = atoi(argv[1]);
   else D = 3; 
   Nth = 4;

   std::cout << "\nCalling constructor " << std::endl;

   // Initialize the thread pool
   // --------------------------
   NP = new NPool(Nth, 20);

   std::cout << "\nAllocating initial task  " << std::endl;

   // Submit single task job, and wait for idle
   // -----------------------------------------
   RecTask *T = new RecTask(1, 1000);
   
   std::cout << "\nSubmitting job  " << std::endl;

   jobID = NP->SubmitJob(T);
   NP->WaitForJob(jobID);
   return 0;
   }

// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// SynchIO.C
// ----------
// This example shows how to use an auxiliary empty task to 
// program in TBB the traditional IO parallel pattern: the main 
// task launches an IO task, does something else, and waits for 
// its termination.
//
// In this example, an empty task and the properties of the TBB 
// task scheduler is used to implement a synchronization pattern 
// that normally requires a Boolean lock.
//
// THIS CODE WORKS
// *********************************************************************
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <Timer.h>
#include <iostream>
#include "tbb/task_scheduler_init.h"
#include "tbb/tbb_stddef.h"
#include "tbb/task.h"

using namespace tbb;

void InputData();

// Globals, read from file "synchIO.dat"
// -----------------------------------
int Nth;
long mainwait, iowait;   // waiting times in milliseconds

// --------------------------------------
// The I/O task class
// This task waits for a given number of
// milisecinds, between the start end end 
// messages
// --------------------------------------
class IOTask : public task
   {
   private:
   long   timewait;
   Timer  T;

   public:
   IOTask(int tw): timewait(tw) {}
   task *execute()
      {
      std::cout << "\n IOtask starts " << std::endl;
      T.Wait(iowait);
      std::cout << "\n IOtask ends " << std::endl;
      return NULL;
      }
   }; 

class WIO : public task
  {
   public:
   task *execute()
      {
      IOTask &t = *new(allocate_child()) IOTask(iowait);
      set_ref_count(2);  
      spawn_and_wait_for_all(t);
      return NULL;
      }
   };


// --------------------------------------------------
// This is an auxiliary template function to submit
// the parallel job. template argument is the work 
// holder task class that encapsulates the job.
// --------------------------------------------------
template <typename T>
empty_task* SubmitJob()
   {
   empty_task* E = new( task::allocate_root() ) empty_task;
   E->set_ref_count(2);
   T *t = new( E->allocate_child() ) T();
   E->spawn(*t);
   return E;
   }


// ---------------------------------------
// Auxiliary function to wait for IOTask
// ---------------------------------------
void WaitForJob(empty_task* E)
   {
   E->wait_for_all();
   E->destroy(*E);
   }


int main(int argc, char **argv)
   {
   Timer T;
   InputData();            // read Nth, mainwait, inputwait
   task_scheduler_init init(Nth);

   std::cout << "\nMain starts "<< std::endl;
   // ---------------------------------------------------
   empty_task *e = SubmitJob<WIO>();  // submit
   T.Wait(mainwait);                  // do somethng else
   WaitForJob(e);                     // wait for job 
   // ---------------------------------------------------
   std::cout << "\nMain ends " << std::endl;
   }

// ----------------------
// Auxiliary I/O function
// ----------------------
void InputData()
    {
    FILE *fp;
    char buffer[128];

    if( !(fp = fopen("synchIO.dat", "r") ))
	   {
	   std::cout << "\n Input error" << std::endl;
	   exit(0);
	   }
	
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &Nth);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &mainwait);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &iowait);
    fclose(fp);
    }
 




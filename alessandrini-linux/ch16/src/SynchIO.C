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
// task scheduler are used to implement a synchronization pattern 
// that normally requires a Boolean lock.
//
// The structure of the code is the same as in the Submit.C example.
// The only difference is that in this case, we want to pass data
// to the submitted job (the duration of the sleep time interval of
// the IO task). The submission function is modified.
//
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
// miliseconds, between the start and end 
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
      T.Wait(timewait);
      std::cout << "\n IOtask ends " << std::endl;
      return NULL;
      }
   }; 

// --------------------------------------------------
// This is an auxiliary function to submit the IOTask
// - The IOTask is spawned as the child of an empty
//   task.
// - This function returns the empty task which is
//   the parent of the IOTask.
// --------------------------------------------------
empty_task* SubmitIOTask(long n)
   {
   empty_task* E = new( task::allocate_root() ) empty_task;
   E->set_ref_count(2);
   IOTask *t = new( E->allocate_child() ) IOTask(n);
   E->spawn(*t);
   return E;
   }

// ---------------------------------------
// Auxiliary function to wait for IOTask
// ---------------------------------------
void WaitForIOTask(empty_task* E)
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

   // Launch IOTask
   // -------------
   empty_task *e = SubmitIOTask(iowait);

   // Do something else
   // -----------------
   T.Wait(mainwait);

   // Wait for IOTask
   // ---------------
   WaitForIOTask(e);
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
 




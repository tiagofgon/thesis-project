// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// Blocking.C
// ---------------
//
// Simple example on blocking on children style.
//
// Main spawns a BasicTask which does nothing except spawning three
// MyTask objects and waiting for their return. Main, in turn, has to 
// wait for the return of the BasicTask 
//
// The three MyTask tasks are of course allocated as children of the
// BasicTask.
// *****************************************************************
#include <Timer.h>
#include <stdlib.h>
#include <iostream>
#include "tbb/task_scheduler_init.h"
#include "tbb/tbb_stddef.h"
#include "tbb/task.h"

using namespace tbb;

// Global, read from command line
// ------------------------------
int N;

class MyTask : public task
   {
   private:
   int    rank;
   Timer  T;

   public:
   MyTask(int r) : rank(r) {}
   task *execute()
      {
      std::cout << "\n Executing MyTask object, rank = " 
                << rank << std::endl;
      T.Wait(1000);
      return NULL;
      }
   }; 


class BasicTask : public task
   {
   private:
   Timer  T;

   public:
   BasicTask() {}
   task *execute()
      {
      // Allocate three MyTasks as my children
      // ------------------------------------
      MyTask* B1 = new(allocate_child() ) MyTask(1);
      MyTask* B2 = new(allocate_child() ) MyTask(2);
      MyTask* B3 = new(allocate_child() ) MyTask(3);

      set_ref_count(4);  // refcount=(3+1) because I wait
   
      spawn(*B1);        // spawn childs
      spawn(*B2);
      spawn_and_wait_for_all(*B3);
      return NULL;
      }
   }; 

int main(int argc, char **argv)
   {
   int n;
   if(argc==2) n = atoi(argv[1]);
   else n = 2;
   Timer T;
   
   task_scheduler_init init(n);
   std::cout << "\nMain starts "<< std::endl;
   
   // -----------------------------------------------------------
   // NOTICE: the main thread IS NOT a TBB task object. Therefore
   // the only thing main can do is to use the STATIC functions
   // of the task class
   // -----------------------------------------------------------
   BasicTask *B = new ( task::allocate_root() ) BasicTask();
   task::spawn_root_and_wait(*B);
   // ------------------------------------------------------
   std::cout << "\nMain ends " << std::endl;
   }





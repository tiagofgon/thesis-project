// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// Continuation.C
// ---------------
//
// Simple example on continuation passing.
//
// Main spawns a BasicTask which does nothing except spawning three
// MyTask objects. 
//
// Since main IS NOT a TBB task, it cannot be the parent (successor) of 
// any task. Then, the only possibility for main is to allocate root tasks.
// Therefore, main will allocate BasicTask as a root task, and spawn and
// wait for it.
//
// BasicTask does not wait, it spawns the three childs and terminates, 
// and is replaced by a continuation task ContTask.
// 
// The three MyTask tasks are of course allocated as children of the
// ContTask. The continuation task is automatically spawned by the
// last child to terminate.
//
// All this is fine, but the program does not work as we would expect.
// Main IS NOT the parent of BasicTask. ConTask inherits the parent of
// BasicTask, which is NULL. So, main is not the successor of ContTask,
// it remains the successor of BasicTask and terminates as soon as 
// BasicTask terminates, without waiting for the rest of the program
// (which executes correctly).
//
// *****************************************************************
#include <Timer.h>
#include <stdlib.h>
#include <iostream>
#include "tbb/task_scheduler_init.h"
#include "tbb/tbb_stddef.h"
#include "tbb/task.h"

using namespace tbb;

class ContTask : public task
   {
   private:
   Timer  T;

   public:
   ContTask() {}
   task *execute()
      {
      std::cout << "\n  -- Executing continuation task" << std::endl;
      T.Wait(1000);
      return NULL;
      }
   }; 

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
      // Allocate the continuation task C
      // --------------------------------
      ContTask* C = new(allocate_continuation()) ContTask();

      // Allocate three MyTasks as children of C
      // ---------------------------------------
      MyTask* B1 = new(C->allocate_child() ) MyTask(1);
      MyTask* B2 = new(C->allocate_child() ) MyTask(2);
      MyTask* B3 = new(C->allocate_child() ) MyTask(3);

      C->set_ref_count(3);  // C is spawned by last child
   
      spawn(*B1);        // spawn childs
      spawn(*B2);
      spawn(*B3);
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

   std::cout << "\nMain ends " << std::endl;
   }


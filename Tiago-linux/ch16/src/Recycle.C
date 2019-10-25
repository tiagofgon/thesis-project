// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// Recycle.C
// ----------
// A simple example showing how tasks can be recycled.
// In this version
//
// ******************************************************
#include <iostream>
#include "tbb/task_scheduler_init.h"
#include "tbb/tbb_stddef.h"
#include "tbb/task.h"
using namespace tbb;


// Global variables
// ----------------
const int  N=4;
const int Niters=4;

class ParentTask : public task
   {
   public:
    task* execute()
        {
        std::cout << "\n I am ParentTask " << std::endl;
        return NULL;
        }
    }; 


class  PWorkTask : public task
   {
   public:
    const int n;
    task*   successor;
    //task*   myparent;

    // Constructor
    // -----------
    PWorkTask(int n_, task *t): n(n_), successor(t) {}

    task* execute()
        {
        std::cout << "\n I am PworkTask number " << n << std::endl;
        successor->decrement_ref_count();
        recycle_as_child_of( *parent() );
        return NULL;
        }
    }; 


// ----------------------------------------------------------
// This is a function called by main. This function iterates
// "niters" times a parallel pattern made out of a parallel
// region where N PWorkTask tasks are executed in parallel,
// followed by a sequential region where the ParentTask is
// executed. This sequential region in fact establishes a 
// barrier among the N PWorkTask tasks
//
// In the different iterations the PWorkTask tasks are
// recycled.
//
// Recycling, as well as barrier synchronizations, are 
// implemented with empty tasks. See the detailed discussion
// in the book.
// ----------------------------------------------------------
void ExecuteIteration(int niters)
   {
   ParentTask *P;
   PWorkTask*    x[N];
   empty_task* e;

   // Allocate tasks
   // --------------
   P = new( task::allocate_root() ) ParentTask();
   e = new( task::allocate_root() ) empty_task();
   for(int n=0; n<N; ++n)
       x[n] = new( e->allocate_child() ) PWorkTask(n, P);

   // HERE STARTS THE RECURRENT PART
   // ------------------------------
   for(int k=0; k<niters; ++k)
      {
      // Set reference counts of successor tasks
      // ---------------------------------------   
      P->set_ref_count(N+1);
      e->set_ref_count(N+1);

      // Spawn N PWorkTasks, and wait for them to complete
      // -------------------------------------------------
      for(int n=0; n<N; ++n) task::spawn(*x[n]);
      P->wait_for_all();
      P->execute();      // P was not spawned
      }

   // destroy tasks
   // --------------
   task::destroy(*P);
   task::destroy(*e);
   for(int n=0; n<N; ++n) task::destroy(*x[n]);
   }


int main(int argc, char **argv)
   {
   task_scheduler_init init(2);
   ExecuteIteration(Niters);
   std::cout << "\nMain exits " << std::endl;
   }





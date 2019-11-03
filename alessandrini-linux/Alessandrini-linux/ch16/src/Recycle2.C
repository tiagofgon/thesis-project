// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// Recycle2.C
// ----------
// Same code architecture as Recycle.C and Recycle1.C
//
// In this version, each iteration of a parallel computation is
// of the form:
//
// parallel region -> barrier -> parallel region -> barrier
//
// The parallel tasks engaged in the parallel regions are all
// recycled in the successive iterations
//
// This example is a template for the MdTBBSch.C code, which 
// adopts exactly this code organization for the molecular dynamics
// trajectory computation.
//
// *********************************************************************
#include <iostream>
#include "tbb/task_scheduler_init.h"
#include "tbb/tbb_stddef.h"
#include "tbb/task.h"
using namespace tbb;


// Global variables
// ----------------
const int  N=4;
const int Niters=4;

class PWorkTask : public task
   {
   public:
    const int n;
    task*     successor;

    // Constructor
    // -----------
    PWorkTask(int n_, task *t): n(n_), successor(t) {}

    task* execute()
        {
        std::cout << "\n I am PWorkTask number " << n << std::endl;
        successor->decrement_ref_count();
        recycle_as_child_of( *parent() );
        return NULL;
        }
    }; 


void ExecuteIteration()
   {
   PWorkTask*    x[N];
   PWorkTask*    y[N];
   empty_task* dummy;
   empty_task* e1, *e2;
   

   // Allocate tasks
   // --------------
   dummy = new( task::allocate_root() ) empty_task();
   e1    = new( task::allocate_root() ) empty_task();
   e2    = new( task::allocate_root() ) empty_task();

   // Allocate tasks for first parallel section, as childs of e1.
   // ----------------------------------------------------------
   for(int n=0; n<N; ++n)
       x[n] = new( e1->allocate_child() ) PWorkTask(n, dummy);
   
   // Allocate tasks for first parallel section, as childs of e2.
   // Use different rank identifiers to distinguish them.
   // ----------------------------------------------------------
   for(int n=0; n<N; ++n)
       y[n] = new( e2->allocate_child() ) PWorkTask(n+10, dummy);


   // HERE STARTS THE RECURRENT PART
   // ------------------------------
   for(int k=0; k<Niters; ++k)
      {
      // Set reference counts of successor tasks
      // ---------------------------------------   
      dummy->set_ref_count(N+1);
      e1->set_ref_count(N+1);

      // Spawn N PWorkTasks, and wait for them to complete
      // -----------------------------------------------
      for(int n=0; n<N; ++n) task::spawn(*x[n]);
      dummy->wait_for_all();

      std::cout << " \n------------------------------- " << std::endl;

      dummy->set_ref_count(N+1);
      e2->set_ref_count(N+1);
      
      // Spawn N PWorkTasks, and wait for them to complete
      // -----------------------------------------------
      for(int n=0; n<N; ++n) task::spawn(*y[n]);
      dummy->wait_for_all();
      
      std::cout << "\n ==========< new iteration >=========\n " << std::endl;
      }

   // destroy tasks
   // --------------
   task::destroy(*dummy);
   task::destroy(*e1);
   task::destroy(*e2);
   for(int n=0; n<N; ++n) 
      {
      task::destroy(*x[n]);
      task::destroy(*y[n]);
      }
   }

int main(int argc, char **argv)
   {
   task_scheduler_init init(2);
   ExecuteIteration();
   std::cout << "\nMain exits " << std::endl;
   }





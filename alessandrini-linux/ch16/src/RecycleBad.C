// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// RecycleBad.C
//
// First task recycling attempt, that does not work. It shows what 
// the problem is that the same parent task cannot be used both for
// barrier synchronization and recycling
// ***************************************************************
#include <iostream>
#include <stdlib.h>
#include "tbb/task_scheduler_init.h"
#include "tbb/tbb_stddef.h"
#include "tbb/task.h"
#include "tbb/mutex.h"
#define   TBB_SET_DEBUG  1
using namespace tbb;


// Global variables
// ----------------
const int  N=4;
int Niters=1;
mutex M;
mutex::scoped_lock lock;

class ParentTask : public task
   {
   public:
    task* execute()
        {
        __TBB_ASSERT( ref_count()==0, NULL);
        std::cout << "\n I am ParentTask " << std::endl;
        return NULL;
        }
    }; 


class  PWorkTask : public task
   {
   public:
    const int n;
    task*   myparent;

    // Constructor
    // -----------
    PWorkTask(int n_): n(n_) {}

    task* execute()
        {
        lock.acquire(M);
        std::cout << "I am Worker task number " << n << std::endl;
        std::cout << myparent->ref_count() << std::endl;
        lock.release();
        // --------------------------------------------------------
        // Uncomment the next line to recycle tasks. Code deadlocks
        // Explanation is given in book
        // --------------------------------------------------------
        //recycle_as_child_of(*myparent);
        return NULL;
        }
    }; 


void ExecutePSection()
   {
   ParentTask *P;
   PWorkTask*    x[N];

   // Allocate tasks
   // --------------
   P = new( task::allocate_root() ) ParentTask();
   for(int n=0; n<N; ++n)
       {
       x[n] = new( P->allocate_child() ) PWorkTask(n);
       x[n]->myparent = P;
       }

   // HERE STARTS THE RECURRENT PART
   // ------------------------------
   for(int k=0; k<Niters; ++k)
      {
      // Set reference counts of successor tasks
      // ---------------------------------------   
      P->set_ref_count(N+1);

      // Spawn N PWorkTasks, and wait for them to complete
      // -------------------------------------------------
      for(int n=0; n<N; ++n) task::spawn(*x[n]);
      P->wait_for_all();
      P->execute();      // P was not spawned
      }

   // destroy tasks
   // --------------
   task::destroy(*P);
   }

int main(int argc, char **argv)
   {
   if(argc==2) Niters = atoi(argv[1]);
   task_scheduler_init init(2);
   ExecutePSection();
   std::cout << "\nMain exits " << std::endl;
   }





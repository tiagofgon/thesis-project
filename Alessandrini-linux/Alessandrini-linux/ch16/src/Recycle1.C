// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// Recycle1.C
// ----------
// Same code architecture as Recycle.C The only difference is
// that in this case the PWorkTask tasks do something relevant:
// adding two vectors
//
// *********************************************************************
#include <iostream>
#include <stdlib.h>
#include "tbb/task_scheduler_init.h"
#include "tbb/tbb_stddef.h"
#include "tbb/task.h"

#define   VECSIZE  100000
using namespace tbb;


// Global variables
// ----------------
const int  N=4;
int Niters;
double A[VECSIZE];
double B[VECSIZE];

class ParentTask : public task
   {
   public:
    task* execute()
        {
        // Check the vector addition for indices 0 and 4
        // ---------------------------------------------
        std::cout << "\n Parent task reporting:" << std::endl;
        std::cout << "A[0] = " << A[0]  << "    "
                  << "A[4] = " << A[4] << std::endl;
        return NULL;
        }
    }; 


class PWorkTask : public task
   {
   private:
    int n;
    int beg, end;

   public:
    ParentTask* successor;
    task*       fparent;

    // Constructor
    // -----------
    PWorkTask(int n_): n(n_) 
       {
       int step = VECSIZE/N;
       beg = n*step;
       end = (n+1)*step;
       if(n==(N-1)) end = VECSIZE;
       }

    task* execute()
        {
        for(int n=beg; n<end; ++n) A[n] += B[n];
        successor->decrement_ref_count();
        recycle_as_child_of(*fparent);
        return NULL;
        }
    }; 


void ExecuteIterations()
   {
   PWorkTask*    x[N];
   ParentTask* P;
   empty_task* e;

   // Allocate tasks
   // --------------
   P = new( task::allocate_root() ) ParentTask();
   e = new( task::allocate_root() ) empty_task();

   for(int n=0; n<N; ++n)
       {
       x[n] = new( task::allocate_root() ) PWorkTask(n);
       x[n]->successor = P;
       x[n]->fparent = e;
       }

   // HERE STARTS THE RECURRENT PART
   // ------------------------------
   for(int k=0; k<Niters; ++k)
      {
      // Set reference counts of successor tasks
      // ---------------------------------------   
      P->set_ref_count(N+1);
      e->set_ref_count(N);

      // Spawn N PWorkTasks, and wait for them to complete
      // -----------------------------------------------
      for(int n=0; n<N; ++n) P->spawn(*x[n]);
      P->wait_for_all();

      // P is not spawned, so execute it
      // -------------------------------
      P->execute();
      }

   // destroy tasks
   // --------------
   task::destroy(*P);
   task::destroy(*e);
   for(int n=0; n<N; ++n) task::destroy(*x[n]);
   }


int main(int argc, char **argv)
   {
   // Initialize number of iterations   
   if(argc==2) Niters = atoi(argv[1]);
   else Niters=6;

   // Initialize vectors
   for(int n=0; n<VECSIZE; n++)
      {
      A[n] = 0.5;
      B[n] = 0.5;
      }

   task_scheduler_init init(2);
   ExecuteIterations();
   std::cout << "\nMain exits " << std::endl;
   }





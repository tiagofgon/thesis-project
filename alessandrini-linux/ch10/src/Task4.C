// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File Task4.C
// 
// Relation between explicit tasks, barriers and the
// taskwait directive
//
// The purpose of this example is to show that OpenMP takes barriers
// into account when scheduling explicit tasks.
//
// The scenario is basically the same as in Task3.C. A parallel region 
// with N threads is launched. All but one hit immediately a barrier. The 
// remaining thread launches a long explicit level 1 task (which in turn 
// launches another long level 2 explicit task) before hitting the barrier. 
//
// The point is to show that the N threads are released only after 
// all the explicit tasks launched before the barrier are completed.
//
// In task3.C, the level 1 task, shorter than the level 2 one,
// terminated before. We add now a taskwait directive in the level 1
// task, forcing it to wait for its level 2 child. We can then verify
// that this task terminates later.
// ---------------------------------------------------------------

#include <iostream>
#include <omp.h>
#include <Timer.h>

void print_message(int r, int l)
   {
   #pragma omp critical
      {
      std::cout << "\n Task level " << l << " executed by thread " << r
                << "\n is terminated" << std::endl;
      }
   }

// Level 2 task
// ------------
void task_level2()
   {
   int rank = omp_get_thread_num();
   Timer T;
   T.Wait(8000);    // wait 8 seconds
   print_message(rank, 2);
   }

// Level 1 task. Launches level 2 task
// -----------------------------------
void task_level1()
   {
   int rank = omp_get_thread_num();
   Timer T;
   T.Wait(3000);    // wait 3 seconds
   
   #pragma omp task untied
       { task_level2(); }
   #pragma omp taskwait      // new, wait for child

   T.Wait(3000);    // wait another seconds
   print_message(rank, 1);
   }
       
// The main function
// -----------------
int main()
   {

   // Set control variables
   // ----------------------
   omp_set_nested(1);
   omp_set_max_active_levels(8);
    
   omp_set_num_threads(4);
   std::cout << "\n Entering parallel region that launches tasks "
             << std::endl;

   #pragma omp parallel 
      {
      int rank = omp_get_thread_num();
      // ------------------------------------------------------
      #pragma omp single
         {
         #pragma omp task untied
            { task_level1(); }
         }
      #pragma omp barrier
      #pragma omp critical
        {
        std::cout << "\n Thread " << rank << " released after barrier "
                << std::endl;
        }
      // -----------------------------------------------------
      }
   }

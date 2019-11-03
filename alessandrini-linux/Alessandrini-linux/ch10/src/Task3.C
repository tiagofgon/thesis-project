// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File Task3.C
// ------------
// Relation between explicit tasks and barriers
//
// NOTES:
// ******
// The purpose of this example is to show that OpenMP takes barriers
// into account when scheduling explicit tasks.
//
// The scenario is the following. A parallel region with N threads is
// launched. All but one hit immediately a barrier. The remaining
// thread launches a long explicit task - which in turn launches 
// another long explicit task - before hitting the barrier. 
//
// The example verifies that the N threads at the barrier are released 
// only after all the explicit tasks launched before the barrier are 
// completed.
//
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
   
   #pragma omp task untied  // launch child level 2
       { task_level2(); }
   
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
            { task_level1(); }   // launch child level 1
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

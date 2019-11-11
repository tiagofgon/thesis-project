// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ***************************************** 
// File IoTaskOmp.C
//
// Demonstrates idle wait synchronization
// in OpenMP.
// - Main launches an IO task and performs a busy 
//   wait, reading the value of a flag.
// - The IO task performs its duty and toggles the
//   flag.
// ---------------------------------------
#include <iostream>
#include <omp.h>
#include <Timer.h>

// Global synchronization variables
// --------------------------------
bool     flag;
omp_lock_t mylock;


// Code for workers tasks
// ----------------------
void io_task()
   {
   Timer T;
   T.Wait(2000);  // perform IO operation
   std::cout << "\n IO operation done. Signaling" << std::endl;
   
   // change flag value
   omp_set_lock(&mylock);
   flag = false;
   omp_unset_lock(&mylock);
   }   

void main_task()
   {
   bool my_flag;
   do
     {
     omp_set_lock(&mylock);
     my_flag = flag;
     omp_unset_lock(&mylock);
     }while(my_flag==true);
   std::cout << "\n Main task released" << std::endl;
   }

void TaskFct()
   {
   int rank = omp_get_thread_num();
   if(rank==0) main_task();
   else io_task();
   }

// ------------------------
// Main, always the same...
// ------------------------

int main(int argc, char **argv)
   {
   flag = true;
   omp_set_num_threads(2);
   #pragma omp parallel
      { TaskFct(); }
   return 0;
   }

// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File IoTaskOmp.C

// Demonstrates busy wait synchronization in OpenMP 
// environent, using the OmpBlock class (full OMP code)
// -------------------------------------------------

#include <iostream>
#include <omp.h>
#include <OmpBLock.h>
#include <Timer.h>

// Global synchronization variable
// -------------------------------

OmpBLock BL(true);

// Code for workers tasks
// ----------------------
void io_task()
   {
   Timer T;
   T.Wait(2000);  // perform IO operation
   std::cout << "\n IO operation done. Signaling" << std::endl;
   
   // change flag value
   BL.SetValue(false);
   }   

void main_task()
   {
   BL.Wait_Until_False();
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
   omp_set_num_threads(2);
   #pragma omp parallel
      { TaskFct(); }
   return 0;
   }

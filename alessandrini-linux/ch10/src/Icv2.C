// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File Icv2.C
// -----------
// Checking default values of ICVs
// Setting some ICVs values
//
// NOTES:
// ******
//
// Setting "nested" is necesary to dispose of more than one thread
// in nested parallel region. 
//
// This program starts with a parallel section of 2 threads. Each
// one of these two threads launches a parallel section of three
// threads. There are altogether 6 threads.
//
// This program checks that there are six distinct threads in the
// program, by printing not only its rank but also its pthread_t
// identifier, which is intrinsic to a thread.
//
// This example shows the map id OpenMP thread numbers (ranks) to
// threads. One worker thread with rank different from 0 in the
// outer parallel region, that launches a nested parallel region,
// becomes master and takes the rank=0 inside the nested region.
// ---------------------------------------------------------------

#include <iostream>
#include <omp.h>

using namespace std;   

int main()
   {

   // Set control variables
   // ----------------------
   omp_set_nested(1);
   omp_set_max_active_levels(8);
   omp_set_dynamic(0);
    
   omp_set_num_threads(2);
   int r1;
   // -------------------------------------------// parallel section
   #pragma omp parallel private(r1)
      {
      r1 = omp_get_thread_num();
    
      omp_set_num_threads(3);
      int r2;
      #pragma omp parallel private(r2)
          // ------------------------------ --nested parallel section
          {    
          r2 = omp_get_thread_num();
          #pragma omp critical
             {   
             cout << "\n I am inner thread " << r2 << " with Pthread ID "
                  << pthread_self() << endl;
             }
          // ---------------------------------------------------------
          }
      
      #pragma omp barrier
      #pragma omp critical
         {   
         cout << "\n I am outer thread " << r1 << " with Pthread ID "
              << pthread_self() << endl;
         }
    
      // -----------------------------------------------------
      }
   }

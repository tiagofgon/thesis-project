// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File Icv1.C
// -----------
// First test of OpenMP
// Checking default values of ICVs
// Setting some ICVs values
//
// NOTES:
// ******
//
// Setting "nested" may be necesary in some OpenMP implementations to 
// dispose of more than one thread in a nested parallel region. 
//
// This program starts with a parallel section of 2 threads. Each
// one of these two threads launches a parallel section of three
// threads. There are altogether 6 threads.
//
// In the inner parallel sections, only one thread writes to stdout.
// We have then 2 inner messages.
//
// In the outer parallel section, only one thread writes to stdout.
// There is therefore only one message
// ---------------------------------------------------------------

#include <iostream>
#include <omp.h>

using namespace std;   

int main()
   {
   // Print default configuration
   // ---------------------------
   cout << "\nInitial thread limit : " << omp_get_thread_limit();
   cout << "\nInitial dynamic threads : " << omp_get_dynamic();
   cout << "\nInitial nested parallel : " << omp_get_nested() << endl;

   // Set control variables
   // ----------------------
   omp_set_nested(1);
   omp_set_max_active_levels(8);
   omp_set_dynamic(0);
    
   omp_set_num_threads(2);
   #pragma omp parallel
      {
      // ---------------------------------- parallel section
      
      omp_set_num_threads(3);
      #pragma omp parallel
          // ------------------------------ nested parallel section
          {    
          omp_set_num_threads(4);
          #pragma omp single
             {
             cout << "\nInner parallel section ";
             cout << "\n---------------------- ";
             cout << "\nMax_active_levels : " << omp_get_max_active_levels();
             cout << "\nNum_threads       : " << omp_get_num_threads();
             cout << "\nMax_threads       : " << omp_get_max_threads() << endl;
             }
          // -------------------------- end of nested parallel section
          }
      
      #pragma omp barrier
      #pragma omp single
          {
          cout << "\nOuter parallel section ";
          cout << "\n---------------------- ";
          cout << "\nMax_active_levels : " << omp_get_max_active_levels();
          cout << "\nNum_threads       : " << omp_get_num_threads();
          cout << "\nMax_threads       : " << omp_get_max_threads() 
               << "\n" << endl;
          }
      // ---------------------------- end of external parallel section
      }
   }

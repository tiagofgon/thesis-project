// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// DbSearchOmp4-A.C
// Models a data base search
//
// This example demonstrates the OpenMP 4.0 parallel region
// cancellation mechanism. A parallel region is launched where 
// each worker thread performs a database search. When one of 
// the parallel tasks finds the target data, the parallel 
// region is cancelled.
// ======================================================

#include <iostream>
#include <Rand.h>
#include <omp.h>
#include <stdlib.h>
#include <math.h>

// This is the data set to be passed to the main
// thread
// ---------------------------------------------
struct Data
   {
   double d;        
   int    rank;    
   };
                   
const double EPS = 0.00000001;
const double target = 0.58248921;
Data D;

// ----------------------------------------------
// The main function. get the number of threads
// from the command line
// ----------------------------------------------

int main(int argc, char **argv)
    {
    int nTh;

    if(argc==2) nTh = atoi(argv[1]);
    else nTh = 2;

    omp_set_num_threads(nTh);
    #pragma omp parallel
       { 
       double d;
       int rank = omp_get_thread_num();
       Rand R(999*(rank+1));
       #pragma omp critical
          { std::cout << "\n Thread " << rank << " starts" << std::endl;}

       while(1)       // infinite loop
          {
          #pragma omp cancellation point parallel  // check for cancellation
          d = R.draw();
          if(fabs(d-target)<EPS)
             {
             std::cout << "\n Target found" << std::endl;
             D.d = d;
             D.rank = rank;
             #pragma omp cancel parallel          // request cancellation
             }
          }
       }

    // Print search result
    // -------------------
    std::cout << "\n Thread " << D.rank << " found value " 
              << D.d << std::endl;

    return 0;
    }
	
/*********************************************************/

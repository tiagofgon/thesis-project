// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File Sections1.C  
// 
//
// Using the "parallel sections" directive to check the 
// behavior of the "barrier" directive.
//
// We fix the number of parallel tasks to 6, and get the number 
// of threads from the command line (default is 6)
//
// Barriers do no block even if the number of threads is smaller 
// than the number of tasks. However, the behavior is not what one 
// would  expect. If there are Nth threads with Ntk tasks, and 
// Nth < Ntk, OpenMP executes the tasks in groups equal or smaller 
// than Nth, and applies the barrier to each group.
// ==============================================================

#include <iostream>
#include <stdlib.h>
#include <Timer.h>
#include <RandInt.h>
#include <omp.h>

void BarrierTask()
    {
    Timer T;
    RandInt R(2000);
    int rank = omp_get_thread_num();

    // Wait for random interval between 1 and 3 seconds
    // ------------------------------------------------
    T.Wait( 1000+R.draw() );
    std::cout << "\n Thread  " << rank <<  " before barrier " 
                 << std::endl;
    #pragma omp barrier
    std::cout << "\n Thread  " << rank <<  " after barrier " 
                 << std::endl;
    }


int main(int argc, char **argv)
    {
    int n, nTh;

    // Get number of threads from command line
    if(argc==2) nTh = atoi(argv[1]);
    else nTh=6;

    #pragma omp parallel sections num_threads(nTh)
       {
       #pragma omp section
           { BarrierTask();}
       #pragma omp section
           { BarrierTask();}
       #pragma omp section
           { BarrierTask();}
       #pragma omp section
           { BarrierTask();}
       #pragma omp section
           { BarrierTask();}
       #pragma omp section
           { BarrierTask();}
       }
    return 0;
    }
	
/////////////////////////////////////////////////////////

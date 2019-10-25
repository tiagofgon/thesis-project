// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// DbSearchOmp4-B.C
// Models a data base search
//
// This example demonstrates the OpenMP 4.0 cancellation
// mechanism for a taskgroup instead of a parallel region. 
// A taskgroup construct is launched where worker threads 
// performs a database search. When one of the parallel tasks 
// finds the target data, the taskgroup is cancelled.
// =======================================================

#include <iostream>
#include <Rand.h>
#include <SafeCounter.h>
#include <omp.h>
#include <stdlib.h>
#include <math.h>

// This is the data set to be passed to the main
// thread. 
// ---------------------------------------------
struct Data
   {
   double d;
   int    rank;
   };
                   
const double EPS = 0.00000001;
const double target = 0.58248921;
Data D;
SafeCounter SC;

// ----------------------------------------------
// The main function. get the number of threads
// and the number of tasks from the command line
// ----------------------------------------------
int main(int argc, char **argv)
    {
    int nTh, nTk;
    
    nTh = 2;
    nTk = 2;
    if(argc==2) nTh = atoi(argv[1]);
    if(argc==3) 
        {
        nTh = atoi(argv[1]);
        nTk = atoi(argv[2]);
        }

    omp_set_num_threads(nTh);
    #pragma omp parallel
       { 
       #pragma omp single
          {
          // This code block is executed by only one thread
          #pragma omp taskgroup
             {
             // *********< start taskgroup construct> **********

             #pragma omp task            // required!
                {
                // This task launches the nTk tasks that 
                // perform the search
                // -------------------------------------
                for(int n=0; n<nTk; ++n)
                   {
                   #pragma omp task
                       {
                       double d;
                       int  rank = SC.Next();
                       Rand R(999*(rank+1));

                       // perform the search
                       // ------------------
                       while(1)
                          {
                          #pragma omp cancellation point taskgroup
                          d = R.draw();
                          if(fabs(d-target)<EPS)
                             {
                             D.d = d;
                             D.rank = rank;
                             #pragma omp cancel taskgroup
                             }
                          } 
                       }       // end of worker task
                    }          // end for
                }              // end of launcher task

             //************< end of taskgroup construct> *************
             }                
          }                    // end single
       }                       // end parallel

    // Print search result
    // -------------------
    std::cout << "\n Thread " << D.rank << " found value " 
              << D.d << std::endl;

    return 0;
    }
	
/*********************************************************/

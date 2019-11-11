// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File DbTbb.C.
//
// This example demonstrates the cancellation of a TBB
// taskgroup.
//
// Database search, using taskgroup cancellation to stop 
// the search when one task finds the target.
// ----------------------------------------------------

#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <Rand.h>
#include "tbb/task_scheduler_init.h"
#include "tbb/tbb_stddef.h"
#include "tbb/task_group.h"

using namespace tbb;

struct Data       // output passed to main thread
   {
   double d;        
   int    rank;    
   };
                   
const double EPS = 0.00000001;
const double target = 0.58248921;
Data D;
task_group tg;    // global task_group

// The workers thread function
// ---------------------------                 
void SearchTask(int n)
    {
    double d;
    Rand R(999*(n+1));
    while(1)       // infinite loop
       {
       if(tg.is_canceling()) break;   // check for cancellation
       d = R.draw();
       if(fabs(d-target)<EPS)
          {
          D.d = d;
          D.rank = n;
          tg.cancel();                // request cancellation
          }
       }
    }

// ----------------------------------------------
// The main function. get the number of threads
// from the command line
// ----------------------------------------------
int main(int argc, char **argv)
    {
    int nTh = 2;
    int nTk = 4;

    // override from command line
    // --------------------------
    if(argc==2) nTh = atof(argv[1]);
    if(argc==3) 
        {
        nTh = atof(argv[1]);
        nTk = atof(argv[2]);
        }

    task_scheduler_init init(nTh);
    for(int n=0; n<nTk; ++n) tg.run([=]{SearchTask(n);});
    tg.wait();

    // Print search result
    // -------------------
    std::cout << "\n Task " << D.rank << " found value " 
              << D.d << std::endl;
    return 0;
    }


// **************************************
// Copyright (c) 2011 Victor Alessandrini
// All rights reserved.
// **************************************
// =====================================================
// DbSearch.C
// Models a data base search
//
// Worker threads search for a data item, and the first 
// that finds it return the value by writing to a global
// variable, and cancels the workers team
// ====================================================

#include <iostream>
#include <SPool.h>
#include <Rand.h>
#include <stdlib.h>
#include <math.h>
#include <CpuTimer.h>

// This is the data set to be passed to the main
// thread
// ---------------------------------------------
struct Data
   {
   double d;
   int    rank;
   };
                 
SPool *TS;     
const double EPS = 0.000000001;
const double target = 0.58248921;
Data D;
CpuTimer TM;

// The workers thread function
// ---------------------------                 
void th_fct(void *arg)
    {
    double d;
    int rank = TS->GetRank();
    Rand R(999*rank);

    for(;;)
       {
       TS->SetCancellationPoint();
       d = R.draw();
       if(fabs(d-target)<EPS)
          {
          D.d = d;
          D.rank = rank;
          TS->CancelTeam();
          }
       }
    }

// ----------------------------------------------
// The main function. get the number of threads
// from the command line
// ----------------------------------------------

int main(int argc, char **argv)
    {
    int nTh;

    if(argc==2) nTh = atoi(argv[1]);
    else nTh = 2;

    std::cout << "\n Database search emulation " << std::endl;

    // Create worker threads
    // --------------
    TS = new SPool(nTh);
   
    // launch worker threads
    // ---------------------
    TM.Start();
    TS->Dispatch(th_fct, NULL);
    TS->WaitForIdle();
    TM.Stop();
    
    // Print data values
    // -----------------
    std::cout << "\n Received value " << D.d << " from thread " 
              << D.rank << std::endl;
	TM.Report();

    delete TS;
    return 0;
    }
	
/*********************************************************/

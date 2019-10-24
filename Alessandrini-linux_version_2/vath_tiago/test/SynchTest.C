// **************************************
// Copyright (c) 2011 Victor Alessandrini
// All rights reserved.
// **************************************
/* =====================================================
 * SynchTest.C
 * test the Synch interface. 
 * ====================================================*/

#include <iostream>
#include <SynchP.h>
#include <Timer.h>
#include <SPool.h>
#include <stdlib.h>

SPool *TH;                      
SynchP<double> B;

void th_fct(void *arg)
    {
    double d;
    int rank = TH->GetRank();
    Timer X;
    for(int n=0; n<2; n++)
       {
       X.Wait(500);
       d = B.Get();
       std::cout << "\nWorker thread " << rank << " got value " << d
                 << std::endl;
       }
    }


// ----------------------------------------------
// The main function. get the number of threads
// from the command line
// ----------------------------------------------

int main(int argc, char **argv)
    {
    int nTh;
    Timer Tm;

    if(argc==2) nTh = atoi(argv[1]);
    else nTh = 2;
    TH = new SPool(nTh);
    std::cout << "\n *** Testing data transfers among threads\n" 
              << std::endl;

    // launch worker threads
    // --------------------- 
    TH->Dispatch(th_fct, NULL);

    // Main thread code
    // ----------------
    Tm.Wait(3000);
    double d = 1.3546;
    for(int n=0; n<2; n++)
       {
       d += 1.0;
       B.Post(d, nTh);
       std::cout << "\n Main : value posted" << std::endl;
       } 

    TH->WaitForIdle();
    std::cout << "\n Main :  worker threads joined" << std::endl; 
    return 0;
    }
	
/*********************************************************/

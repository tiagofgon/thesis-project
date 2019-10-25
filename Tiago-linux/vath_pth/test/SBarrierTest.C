// **************************************
// Copyright (c) 2011 Victor Alessandrini
// All rights reserved.
// **************************************
/* ==============================================================
 * TestBLock.C
 * test the bLock interface. We start by watching the behavior of
 * the timed waits.
 * =============================================================*/

#include <iostream>
#include <stdlib.h>
#include <Timer.h>
#include <SPool.h>
#include <SpBarrier.h>

SPool     *TS;
SpBarrier *B;

void th_fct(void *arg)
    {
    Timer T;
    T.Wait(500);
    std::cout << "\n From thread: before barrier  " << std::endl;
    B->Wait();
    std::cout << "\n From thread : after first barrier " << std::endl;
    B->Wait();
    std::cout << "\n From thread : after second barrier " << std::endl;
    }

int main(int argc, char **argv)
    {
    int nTh;
    if(argc==1) nTh=2;
    else nTh = atoi(argv[1]);

    std::cout << "\n Testing the spin barrier SpBarrier class " << std::endl;
    TS = new SPool(nTh);
    B = new SpBarrier(nTh);

    TS->Dispatch(th_fct, NULL);
    TS->WaitForIdle();
    return 0;
    }
	

/*********************************************************/

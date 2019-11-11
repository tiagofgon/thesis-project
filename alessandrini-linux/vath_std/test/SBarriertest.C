// **************************************
// Copyright (c) 2011 Victor Alessandrini
// All rights reserved.
// **************************************
/* ==============================================================
 * SBarriertest.C
 * Testing the SpBarrier class.
 * =============================================================*/

#include <iostream>
#include <stdlib.h>
#include <Timer.h>
#include <SPool.h>
#include <SpBarrier.h>

SPool *TS;
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

    TS = new SPool(nTh);
    B = new SpBarrier(nTh);
    std::cout << "\n *** Testing the SpBarrier class\n " << std::endl;

    TS->Dispatch(th_fct, NULL);
    TS->WaitForIdle();
    return 0;
    }
	

/*********************************************************/

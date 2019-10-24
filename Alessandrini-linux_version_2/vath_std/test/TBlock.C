// **************************************
// Copyright (c) 2011 Victor Alessandrini
// All rights reserved.
// **************************************
/* ==============================================================
 * TBlock.C
 * test the bLock interface. We start by watching the behavior of
 * the timed waits.
 * =============================================================*/

#include <iostream>
#include <stdlib.h>
#include <BLock.h>
#include <Timer.h>
#include <SPool.h>

SPool *TS;
BLock *B;

void th_fct(void *arg)
    {
    Timer T;
    T.Wait(8000);
    B->Set_And_Notify(true);
	T.Wait(1000);
	std::cout << "\n From thread : boolean flag set and notified" << std::endl;
    }

int main(int argc, char **argv)
    {
    int status;
    Timer T;
    TS = new SPool(1);

    std::cout << "\n *** Testing timed waits with BLock class\n " 
              << std::endl;

    B = new BLock(false);
    TS->Dispatch(th_fct, NULL);
	std::cout << "\n Main waits for event" << std::endl;
    B->Wait_Until(true, 0L);
	std::cout << " \n Main waits for idle" << std::endl;
    TS->WaitForIdle();
	std::cout << "\n Main exits" << std::endl;
    delete TS;
    delete B;
    return 0;
    }
	

/*********************************************************/

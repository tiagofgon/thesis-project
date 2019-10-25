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

SPool TS(1);
BLock *B;

void th_fct(void *arg)
    {
    Timer T;
    T.Wait(5000);
    B->Set_And_Notify(true);
    std::cout << "\n From thread : change set and notified " << std::endl;
    }

int main(int argc, char **argv)
    {
    int status;
    std::cout << "\n Testing timed waits in TBlock class " << std::endl;

    B = new BLock(false);
    TS.Dispatch(th_fct, NULL);
    do
       {
       status = B->Wait_Until(true, 1000);
       std::cout << "\n From main : got return value = " << status 
                 << std::endl;
       } while(status==0);
    TS.WaitForIdle();
    delete B;
    return 0;
    }
	

/*********************************************************/

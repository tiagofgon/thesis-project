// **************************************
// Copyright (c) 2011 Victor Alessandrini
// All rights reserved.
// **************************************
/* ==============================================================
 * TBlock.C
 * test the bLock interface. We start by watching the behavior of
 * the timed waits.
 *
 * The worker thread performs a 2 seconds treatment, and signals
 * the event.
 *
 * The main thread waits for the event for a "timewait" duration
 * passed from the command line (default is 0, unlimited wait). If 
 * the wait duration is less than 2s, main is timed out. Otherwise, 
 * main receives the notification.
 * =============================================================*/

#include <iostream>
#include <BLock.h>
#include <Timer.h>
#include <stdlib.h>
#include <thread>

BLock *B;

void th_fct()
    {
    Timer T;
    T.Wait(2000);
    B->Set_And_Notify(true);
    std::cout << "\n From thread : change set and notified " << std::endl;
    }

int main(int argc, char **argv)
    {
    int status;
    long timewait;
    std::cout << "\n Testing timed waits in TBlock class " << std::endl;

    if(argc==2) timewait = atol(argv[1]);
    else timewait = 0;                   // unlimited wait

    B = new BLock(false);
    std::thread WT(th_fct);
    status = B->Wait_Until(true, timewait);
    if(status) std::cout << " from main: notification received " << std::endl;
    else std::cout << "from main: wait call timed out " << std::endl;

    WT.join();
    delete B;
    return 0;
    }
	

/*********************************************************/

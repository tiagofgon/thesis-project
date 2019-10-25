// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
/* TBlock.C
 * Test the BLock interface. 
 *
 * This example deals with the behavior of timed waits.
 * The main thread keeps waiting for one second time intervals
 * a notification that must come from the worker thread after
 * about 5 seconds.
 * =============================================================*/

#include <iostream>
#include <stdlib.h>
#include <BLock.h>
#include <Timer.h>
#include <SPool.h>

SPool  TP(1);
BLock *B;

void th_fct(void *arg)
    {
    Timer T;
    T.Wait(5000);
    B->Set_And_Notify(true);
    std::cout << "\n From thread : change notified " << std::endl;
    }

int main(int argc, char **argv)
    {
    int status;

    B = new BLock(false);
    TP.Dispatch(th_fct, NULL);
    do
       {
       status = B->Wait_Until(true, 1000);
       std::cout << "\n From main : got return value = " << status 
                 << std::endl;
       } while(status==0);
    TP.WaitForIdle();
    delete B;
    return 0;
    }
	

/*********************************************************/

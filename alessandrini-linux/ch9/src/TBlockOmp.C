// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
/* TBlock_omp.C
 *
 * This example deals with the behavior of timed waits.
 * The main thread keeps waiting for one second time intervals
 * a notification that must come from the worker thread after
 * about 5 seconds.
 *
 * Uses OpenMP threads.
 * =============================================================*/

#include <iostream>
#include <stdlib.h>
#include <BLock.h>
#include <Timer.h>
#include <omp.h>

BLock *B;

void ThFct()
    {
    Timer T;
    T.Wait(5000);
    B->Set_And_Notify(true);
    std::cout << "\n From thread : change set and notified " << std::endl;
    }

void MainFct()
    {
    int status;
    do
       {
       status = B->Wait_Until(true, 1000);
       std::cout << "\n From main : got return value = " << status 
                 << std::endl;
       } while(status==0);
    }


int main(int argc, char **argv)
    {
    int status;
    B = new BLock(false);

    #pragma omp parallel sections num_threads(2)
       {
       #pragma omp section
          MainFct();
       #pragma omp section
          ThFct();
       }

    delete B;
    return 0;
    }
	

/*********************************************************/

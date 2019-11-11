// **************************************
// Copyright (c) 2011 Victor Alessandrini
// All rights reserved.
// **************************************
/* ==============================================================
 * BLtest_omp.C
 * test the bLock interface. We start by watching the behavior of
 * the timed waits.
 * Use OpenMP thread pool.
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
    B->SetValue(true);
    std::cout << "\n From thread : boolean flag set " << std::endl;
    B->Notify();
    std::cout << "\n From thread : change notified " << std::endl;
    }

void MainFct()
    {
    int status;
    do
       {
       status = B->Wait_Until_True(1000);
       std::cout << "\n From main : got return value = " << status 
                 << std::endl;
       } while(status==0);
    }


int main(int argc, char **argv)
    {
    int status;
    B = new BLock(false);
    
    std::cout << "\n Testing BLock class with OpenMP threads " 
                 << std::endl;

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

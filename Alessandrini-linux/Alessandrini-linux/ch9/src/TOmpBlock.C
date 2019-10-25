// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// TOmpBlock.C
//
// Tests the OpenMP BLock interface: 
// Launching a long IO task. Remember that there are
// no timed waits for busy waits: threas in all cases
// wait forever for the change in predicate
// =================================================

#include <iostream>
#include <stdlib.h>
#include <omp.h>
#include <OBLock.h>
#include <Timer.h>

OBLock *B;

void th_fct()
    {
    Timer T;
    int rank = omp_get_thread_num();
    if(rank==0)
       {
       B->Wait_Until(true);
       std::cout << "\n From main thread joined IO thread"  
                 << std::endl;
       }
    else
       {
       T.Wait(5000);
       B->SetState(true);
       std::cout << "\n From thread : IO done, boolean flag set " 
                 << std::endl;
       }
    }


int main(int argc, char **argv)
    {
    B = new OBLock(false);
    omp_set_num_threads(2);

    #pragma omp parallel
       {  th_fct();  }

    delete B;
    return 0;
    }
	

// **************************************************************

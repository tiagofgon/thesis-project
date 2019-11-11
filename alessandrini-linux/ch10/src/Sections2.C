// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File Sections2.C  
// -----------------
//
// Using the "parallel sections" directive to
// test the BLock interface. The directive is used
// to submit different implicit tasks to the OpenMP
// thread pool.
// =================================================

#include <iostream>
#include <stdlib.h>
#include <BLock.h>
#include <Timer.h>
#include <omp.h>


BLock *B;

void TaskIO()
    {
    Timer T;
    T.Wait(5000);
    B->Set_And_Notify(true);
    std::cout << "\n From thread : change notified " << std::endl;
    }

void MainTask()
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
          MainTask();
       #pragma omp section
          TaskIO();
       }
    delete B;
    return 0;
    }
	
/////////////////////////////////////////////////////////

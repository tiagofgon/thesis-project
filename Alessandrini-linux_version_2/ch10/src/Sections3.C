// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File Sections3.C
// 
// Using the SynchP interface, in a OpenMP context.
// This example shows that the C++ synchronization classes 
// interoperate with OpenMP, and can be used with OpenMP
// managed threads.
//
// This example also shows how the "parallel sections" 
// directive is used to submit different implicit tasks 
// to the OpenMP thread pool.
// ===================================================

#include <stdlib.h>
#include <iostream>
#include <SynchP.h>
#include <Timer.h>
#include <omp.h>

SynchP<double> B;

void WriteTaskFunction()
    {
    Timer X;
    X.Wait(3000);
    double d = 1.3546;
    for(int n=0; n<3; n++)
       {
       d += 1.0;
       B.Post(d, 2);
       std::cout << "\n Writer thread : value posted" << std::endl;
       }
    } 

void ReadTaskFunction()
    {
    double d;
    int rank = omp_get_thread_num();
    Timer X;
    for(int n=0; n<3; n++)
       {
       X.Wait(500);
       d = B.Get();
       std::cout << "\n Reader thread " << rank << 
                  " got value " << d << std::endl;
       }
   }

// ----------------------------------------
// This is the main function
// The number of threads is hardwired to 3
// ----------------------------------------
int main(int argc, char **argv)
    {
    #pragma omp parallel sections num_threads(3) 
       {
       #pragma omp section 
           WriteTaskFunction();
       #pragma omp section 
           ReadTaskFunction();
       #pragma omp section 
           ReadTaskFunction();
       }
    return 0;
    }
	
///////////////////////////////////////////////////////////////

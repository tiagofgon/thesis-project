// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
/* TOmpSynch.C
 *
 * Test of the OSynchP class. 
 *
 * Main launches nTh worker threads, who will get values
 * posted by main. The number of threads is passed by the
 * command line (default is 2).
 *
 * Main posts the double value 2.3546 and the two worker
 * threads read them.
 * Then, main posts the double value 3.3546 and the two 
 * worker threads read them.
 * ******************************************************/

#include <iostream>
#include <OSynchP.h>
#include <omp.h>
#include <stdlib.h>
                   
OSynchP<double> B;

void reader_thread()
    {
    double d;
    int rank = omp_get_thread_num();
    
    // Read loop. Each worker thread will read two times
    // -------------------------------------------------
    for(int n=0; n<2; n++)
       {
       d = B.Get();
       std::cout << "\nWorker thread " << rank << " got value " << d
                 << std::endl;
       }
    }


void writer_thread()
    {
    double d = 1.3546;

    // Write loop. Main posts two values to all threads
    // ------------------------------------------------
    for(int n=0; n<2; n++)
       {
       d += 1.0;
       B.Post(d, 2);
       } 
     }


// ----------------------------------------------
// The main function. get the number of threads
// from the command line
// ----------------------------------------------

int main(int argc, char **argv)
    {
    // launch worker threads
    // --------------------- 
    omp_set_num_threads(3);
    #pragma omp parallel sections
        {
        #pragma omp section
           writer_thread();
        #pragma omp section
           reader_thread();
        #pragma omp section
           reader_thread();
        }
    std::cout << "\n Main :  worker threads joined" << std::endl; 
    return 0;
    }
	
/*********************************************************/

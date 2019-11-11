// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
/* TSynchOmp.C
 * Testing the Synch interface, but using OpenMP threads.
 *
 * Main launches nTh worker threads, who will get values
 * posted by main. The number of threads is passed by the
 * command line (default is 2).
 *
 * Main posts the double value 2.3546 and the two worker
 * threads read them.
 * Then, main posts the double value 3.3546 and the two 
 * worker threads read them.
 *
 * Using the OpenMP environment
 * ====================================================*/

#include <iostream>
#include <sstream>
#include <SynchP.h>
#include <Timer.h>
#include <omp.h>
#include <SafeCout.h>

SynchP<double> B;
SafeCout SC;

void th_fct(void *arg)
    {
    std::ostringstream os;
    double d;
    int rank = omp_get_thread_num();
    Timer X;

    if(rank>0)
       {
       // Workers code
       // ------------
       for(int n=0; n<2; n++)
          {
          X.Wait(500);
          d = B.Get();
          os << "\nWorker thread " << rank << " got value " << d;
          SC.Flush(os);
          }
        }
    else
        {
        // Master code
        // -----------
        X.Wait(3000);
        d = 1.3546;
        for(int n=0; n<2; n++)
           {
           d += 1.0;
           B.Post(d, 2);
           os << "\n Main : value posted";
           SC.Flush(os);
           }
        } 
    }


// ----------------------------------------------
// The main function. get the number of threads
// from the command line
// ----------------------------------------------

int main(int argc, char **argv)
    {
    int nTh;

    omp_set_num_threads(3);    
    #pragma omp parallel
        {
        th_fct(NULL);
        }
    std::cout << "\n Main :  worker threads joined" << std::endl; 
    return 0;
    }
	
/*********************************************************/

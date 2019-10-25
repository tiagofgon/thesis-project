// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// DbSearch_P.C
//
// Models the data base search example of chapter 3. Rather
// than using the built in cancellation service in SPool, we
// explicitly synchronize the threads using a reader-writer
// lock. 
//
// Threads keep reading a flag to detect a cancellation request, 
// and there is only one write coming from the cancelling thread. 
// This is therefore a good case for a rw lock, that allows the
// reads to proceed concurrently, reducing mutual exclusion 
// overhead. The RWLock (reader-writer lock) class is used. It
// encapsulates the native Pthreads or Windows reader-writer locks.
//
// Worker threads search for a data item, and the first 
// that finds it return the value by writing to a global
// variable, and cancels the workers team.
// ====================================================

#include <iostream>
#include <SPool.h>
#include <Rand.h>
#include <stdlib.h>
#include <math.h>
#include <CpuTimer.h>
#include <RWLock.h>
#include <Timer.h>

// This is the data set to be passed to the main
// thread
// ---------------------------------------------
struct Data
   {
   double d;
   int    rank;
   };
                 
const double EPS = 0.00000001;
const double target = 0.58248921;

SPool    *TS;     
Data     D;
CpuTimer T;
RWLock   RW;     // protects cancel_flag
bool     cancel_flag;

// The workers thread function
// ---------------------------                 
void th_fct(void *arg)
    {
    double d;
    long   n;
    bool my_flag;

    int rank = TS->GetRank();
    Rand R(999*rank);
    Timer T;

    n = 0;
    for(;;)
       {
       RW.Lock(false);        // check cancellation request
       my_flag = cancel_flag;
       RW.Unlock(false);
       if(my_flag) break;
       // ------------------
       d = R.draw();
       if(fabs(d-target)<EPS)
          {
          D.d = d;
          D.rank = rank;
          // --------------------
          RW.Lock(true);       // signal cancellation
          cancel_flag = true,
          RW.Unlock(true);
          break;
          // --------------------
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

    if(argc==2) nTh = atoi(argv[1]);
    else nTh = 4;
    cancel_flag = false;

    // Create worker threads
    // --------------
    TS = new SPool(nTh);
   
    // launch worker threads
    // --------------------- 
    T.Start();
    TS->Dispatch(th_fct, NULL);
    TS->WaitForIdle();
    T.Stop();
    
    // Print data values
    // -----------------
    std::cout << "\n Received value " << D.d << " from thread " 
              << D.rank << std::endl;
    T.Report();

    delete TS;
    return 0;
    }
	
/*********************************************************/

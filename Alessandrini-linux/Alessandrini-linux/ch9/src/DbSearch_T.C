// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// DbSearch_T.C
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

// Worker threads search for a data item, and the first 
// that finds it return the value by writing to a global
// variable, and cancels the workers team.
//
// THIS CODE USES THE TBB RWLOCK INSTEAD OF THE RWLock class.
//
// NOTICE: there is no RWLock in C++11. In a C++11 environment, 
// use the TBB RW lock.
// ============================================================

#include <iostream>
#include <SPool.h>
#include <Rand.h>
#include <tbb/spin_rw_mutex.h>
#include <stdlib.h>
#include <math.h>
#include <CpuTimer.h>

// This is the data set to be passed to the main
// thread
// ---------------------------------------------
struct Data
   {
   double d;
   int    rank;
   };
                 
SPool *TS;     
const double EPS = 0.00000001;
const double target = 0.58248921;
Data D;

tbb::spin_rw_mutex my_mutex;         // protects interrupt flag
bool   interrupt;

// The workers thread function
// ---------------------------                 
void th_fct(void *arg)
    {
    double d;
    bool my_flag;
    int rank = TS->GetRank();
    Rand R(999*rank);

    for(;;)
       {
       // -------------------------------------------
       // Set here an interruption point, by checking 
       // the interrupt flag
          {
          tbb::spin_rw_mutex::scoped_lock sc(my_mutex, false);
          my_flag = interrupt;
          }
       if(my_flag) break;
       // ------------------------------------------

       d = R.draw();
       if(fabs(d-target)<EPS)
          {
          D.d = d;
          D.rank = rank;

          // -----------------------------
          // Signal end of task, and break
             {
             tbb::spin_rw_mutex::scoped_lock sc(my_mutex, true);
             interrupt = true;
             }
          break;
          // -----------------------------
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
    CpuTimer T;

    if(argc==2) nTh = atoi(argv[1]);
    else nTh = 2;
    interrupt = false;

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

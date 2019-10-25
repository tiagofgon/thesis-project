// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// DbSearch_S.C
// Models a data base search
//
// Worker threads search for a data item, and the first 
// that finds it returns the value by writing to a global
// variable, and cancels the workers team.
//
// In this example, we do not use the built-in SPool job
// cancellation facility. We cancel explicitly. Tasks
// keep reading a "cancel" flag until they read "true",
// in which case they exit. The task that finds the
// searched value sets "cancel" to true, and exits.
//
// We have in this case an overwhelming number of reads
// and one write. Since several threads are reading, the
// read must be mutex protected. Two jobs are executed, 
// and their performances measured:
//
// 1) Readers and writer access "cancel" by locking an
//    ordinary mutex
//
// 2) An atomic flag "a_cancel" is used instead
//
// ====================================================

#include <iostream>
#include <SPool.h>
#include <Rand.h>
#include <stdlib.h>
#include <atomic>
#include <math.h>
#include <mutex>
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
CpuTimer T;

bool cancel = false;
std::mutex my_mutex;

std::atomic<int> a_cancel;

// The workers thread function, atomic flag 
// ----------------------------------------                 
void th_fct2(void *arg)
    {
    double d;
    int rank = TS->GetRank();
    Rand R(999*rank);

    for(;;)
       {
       // Test for cancellation request
       // ----------------------------- 
       if(a_cancel==1) break;
       
       // Compute again
       // -------------   
       d = R.draw();
       if(fabs(d-target)<EPS)
          {
          D.d = d;
          D.rank = rank;
          // set cancellation request
          // ------------------------
          a_cancel = 1;
          }
       }
    }


// The workers thread function, reader-writer lock
// -----------------------------------------------                 
void th_fct1(void *arg)
    {
    double d;
    int rank = TS->GetRank();
    Rand R(999*rank);

    for(;;)
       {
       // Test for cancellation request
       // ----------------------------- 
       bool test_cancel;
          {
          std::lock_guard<std::mutex> lock(my_mutex);
          test_cancel = cancel;
          }
       if(test_cancel==true) break;
       
       // Compute again
       // -------------   
       d = R.draw();
       if(fabs(d-target)<EPS)
          {
          D.d = d;
          D.rank = rank;
             // set cancellation request
             // ------------------------
             {
             std::lock_guard<std::mutex> lock(my_mutex);
             cancel = true;
             }     
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
    else nTh = 2;

    // Create worker threads
    // --------------
    TS = new SPool(nTh);
   
    // launch first job: ordinary mutexes 
    // ---------------------------------- 
    T.Start();
    TS->Dispatch(th_fct1, NULL);
    TS->WaitForIdle();
    T.Stop();
    
    // Print data values
    // -----------------
    std::cout << "\n Ordinary mutex results: "; 
    std::cout << "\n Received value " << D.d << " from thread " 
              << D.rank << std::endl;
    T.Report();

    // launch second job : RWLock mutex 
    // ---------------------------------- 
    a_cancel = 0;
    T.Start();
    TS->Dispatch(th_fct2, NULL);
    TS->WaitForIdle();
    T.Stop();
    
    // Print data values
    // -----------------
    std::cout << "\n RWLock results: " ;
    std::cout << "\n Received value " << D.d << " from thread " 
              << D.rank << std::endl;
    T.Report();

    delete TS;
    return 0;
    }
	
/*********************************************************/

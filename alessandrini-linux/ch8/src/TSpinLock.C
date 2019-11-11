// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// TSpinLock.C
// ----------
//
// The SpinLock class encapsulates the native spin mutex in
// Pthreads, and is a custom class using the std::atomic
// services in C++11.
//
// Testing the SpinLock in a context of important mutex
// contention: the example that models a data base search
// (same logic as the DbSSearch example in chapter 3)
// ================================================

#include <iostream>
#include <SpinLock.h>
#include <SPool.h>
#include <Rand.h>
#include <stdlib.h>
#include <math.h>
#include <CpuTimer.h>

// ----------------------------------------------------
// This is a simple utility class that reads and writes
// a boolean flag protected with a SpinLock mutex.
// ----------------------------------------------------
class InterruptFlag
   {
   private:
    bool flag;
    SpinLock flag_mutex;

   public:
    InterruptFlag() : flag(false) {}

    void SetFlag(bool B)
       {
       flag_mutex.Lock();
       flag = B;
       flag_mutex.Unlock();
       }   
                 
    bool GetFlag()
       {
       bool retval;
       flag_mutex.Lock();
       retval = flag;
       flag_mutex.Unlock();
       return retval;
       }
    };   
                 
SPool *TS;     
InterruptFlag IF;

// Notice: InterruptFlag has its own built in SpinLock
// The SpinLock that follows is used to construct a
// critical section when the result is found.
// ----------------------------------------------------
SpinLock mymutex;
double result;     // data passed to main thread
int    out_rank;   // idem

const double EPS = 0.000000001;
const double target = 0.58248921;

// The workers thread function
// ---------------------------                 
void th_fct(void *arg)
    {
    double d;
    int rank = TS->GetRank();
    Rand R(999*rank);

    do
       {
       for(int n=0; n<10; ++n)
          {
          d = R.draw();
          if(fabs(d-target)<EPS)
             {
             mymutex.Lock();
             if(IF.GetFlag()==false)
                {
                IF.SetFlag(true);
                result = d;
                out_rank = rank;
                }
             mymutex.Unlock();
             }
          }
       }while(IF.GetFlag()==false);
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

    TS = new SPool(nTh);
    CpuTimer Tm;

    // launch worker threads
    // --------------------- 
    Tm.Start();
    TS->Dispatch(th_fct, NULL);
    TS->WaitForIdle();
    Tm.Stop();
    
    // Print data values
    // -----------------
    std::cout << "\n Received value " << result << " from thread " 
              << out_rank << std::endl;
    Tm.Report();
    delete TS;
    return 0;
    }
	
/*********************************************************/

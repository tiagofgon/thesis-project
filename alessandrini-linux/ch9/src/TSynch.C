// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
/* TSynch.C
 * Test of the SynchP interface. 
 *
 * Main launches nTh worker threads, who will get values
 * posted by main. The number of reader threads is passed 
 * by the command line (default is 2).
 *
 * Main posts the double value 2.3546 and the two worker
 * threads read it.
 * Then, main posts the double value 3.3546 and the two 
 * worker threads read it.
 *
 * ====================================================*/

#include <iostream>
#include <sstream>
#include <SynchP.h>
#include <Timer.h>
#include <SPool.h>
#include <SafeCout.h>
#include <stdlib.h>

SPool *TH;                      
SynchP<double> B;
SafeCout SC;

void th_fct(void *arg)
    {
    std::ostringstream os;
    double d;
    int rank = TH->GetRank();
    Timer X;
    
    // Read loop. Each worker thread will read two times
    // -------------------------------------------------
    for(int n=0; n<2; n++)
       {
       X.Wait(500);
       d = B.Get();
       os << "\nWorker thread " << rank << " got value " << d;
       SC.Flush(os);
       }
    }

// ----------------------------------------------
// The main function. get the number of threads
// from the command line
// ----------------------------------------------
int main(int argc, char **argv)
    {
    int nTh;
    Timer Tm;
    std::ostringstream os;

    if(argc==2) nTh = atoi(argv[1]);
    else nTh = 2;
    TH = new SPool(nTh);
   
    // launch worker threads
    // --------------------- 
    TH->Dispatch(th_fct, NULL);

    // Main thread code
    // ----------------
    Tm.Wait(3000);
    double d = 1.3546;

    // Write loop. Main posts two values to all threads
    // ------------------------------------------------
    for(int n=0; n<2; n++)
       {
       d += 1.0;
       B.Post(d, nTh);
       os << "\n Main : value posted";
       SC.Flush(os);
       } 

    TH->WaitForIdle();
    os << "\n Main :  worker threads joined";
    SC.Flush(os); 
    return 0;
    }
	
/*********************************************************/

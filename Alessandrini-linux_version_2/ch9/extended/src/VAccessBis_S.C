// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// VAccess_P.C
//
// Uses Reader-Writer lock to implement thread safety in
// write-read access to a stl::vector<int> container.
// We are using here the C++11 implementation of the
// RWLock class 
// 
// A Timer is used to slow down threads
//
// The main thread uses the BLock B to stop an infinite read 
// loop executed by reader threads.
//
// There are TWO THREAD POOLS, with nTh threads each: 
// one pool TR for reader tasks, and another pool TW for 
// writer tasks. Main synchronizes both pools with the
// BLock B. 
//
// The outlock mutex is used to order writes to stdout.
// -----------------------------------------------------

#include <RWLock.h>
#include <Timer.h>
#include <SPool.h>
#include <vector>
#include <mutex>
#include <print_container.h>
#include <stdio.h>
#include <stdlib.h>
#include <CpuTimer.h>

void InputData();

using namespace std;

std::vector<int> V_RW, V_M;
int    nTh, main_wait;
RWLock RW;
std::mutex outlock;
std::mutex my_mutex;
SPool *TR, *TW;
int counter;

// -------------------------------------------------
// Code executed by reader threads, using RWLock
// Reader threads:
// 1 - Wait for 250 ms
// 2 - Read and print the container content to stdin
//     This operation is stretched by an additional
//     Timer wait.
// -------------------------------------------------
void RThread_RW(void *P)
   {  
   int n, rank;
   char buff[128];
   Timer T;
   int my_counter;

   rank = TR->GetRank();
   do
      {
      T.Wait(250); 
      RW.Lock(false);
      T.Wait(1000); 
      my_counter = counter;
      std::vector<int> VV = V_RW;    // this is the read
      // ------------------------
         {
         std::lock_guard<std::mutex> lock(outlock);
         print_container(VV, " ", 20);
         }
      // ------------------------
      RW.Unlock(false);
      }while(my_counter<(6*nTh));
   } 


/* ---------------------------------------------
 * Code executed by writer threads, using RWLock
 * A rank k thread repeats 5 times the following 
 * steps
 * 1) perform a timed wait randomly selected in 
 *    [0, 2000] miliseconds, to delay the thread
 * 2) Add a new value to the vector container.
 *    The value addad is 10*rank+n
 * -------------------------------------------*/

void WThread_RW(void *P)
   {
   int n, rank, msec, value;
   Timer T;
   rank = TW->GetRank();
   value = 10*rank;

   for(n=1; n<=6; n++)
      {
      msec = rand()%2000;
      T.Wait(msec);

      RW.Lock(true);
      // ------------------------
      value++;
      counter++;
      V_RW.push_back(value);
      // ------------------------
      RW.Unlock(true);
      }
   }

// -------------------------------------------------
// Code executed by reader threads, using normal
// mutual exclusion (mutex locking).
// Reader threads:
// 1 - Wait for 250 ms
// 2 - Lock mutex, adnd read and print the container 
//     content to stdin. This operation is stretched
//     by an additional Timer wait
// -------------------------------------------------
void RThread_M(void *P)
   {  
   int n, rank;
   char buff[128];
   Timer T;
   int my_counter;

   rank = TR->GetRank();
   do
      {
      T.Wait(250);
         {
         std::lock_guard<std::mutex> lock(outlock);
         T.Wait(1000); 
         my_counter = counter;
         std::vector<int> VV = V_M;    // this is the read
         print_container(VV, " ", 20);
         }
      }while(counter<(6*nTh));
   } 

/* ---------------------------------------------
 * Code executed by writer threads, using ordinary
 * mutex locking
 * A rank k thread repeats 5 times the following 
 * steps
 * 1) perform a timed wait randomly selected in 
 *    [0, 2000] miliseconds, to delay the thread
 * 2) Add a new value to the vector container.
 *    The value addad is 10*rank+n
 * -------------------------------------------*/

void WThread_M(void *P)
   {
   int n, rank, msec, value;
   Timer T;
   rank = TW->GetRank();
   value = 10*rank;

   for(n=1; n<=6; n++)
      {
      msec = rand()%2000;
      T.Wait(msec);
         {
         std::lock_guard<std::mutex> lock(outlock);
         value++;
         counter++;
         V_M.push_back(value);
         }
      }
   }


main(int argc, char **argv)
   {
   int n, status;
   Timer T;
   CpuTimer TM;
   char ch;

   InputData();
	
   TR = new SPool(nTh);
   TW = new SPool(nTh);

   // ----------------------------------------------
   // main() launches the reader and writer threads
   // using the RWLock
   // Insert some values in the V_RW vector container
   // ----------------------------------------------
   for(int n=1; n<=5; ++n) V_RW.push_back(n);

   counter = 0;
   TM.Start();
   TR->Dispatch(RThread_RW, NULL);
   TW->Dispatch(WThread_RW, NULL);
   // ---------------------------
   TR->WaitForIdle();
   TW->WaitForIdle();
   TM.Stop();
   std::cout << "\n Computation with RWLock : " << std::endl;
   TM.Report();

   std::cout << "\n Enter a char to continue: ";
   std::cin >> ch;

   // ---------------------------------------------------
   // Next, main() launches the reader and writer threads
   // using ordinary mutex locking
   // Insert some values in the V_M vector container
   // ----------------------------------------------
   for(int n=1; n<=5; ++n) V_M.push_back(n);
   counter = 0;
   TM.Start();
   TR->Dispatch(RThread_M, NULL);
   TW->Dispatch(WThread_M, NULL);
   // -----------------------------------
   TR->WaitForIdle();
   TW->WaitForIdle();
   TM.Stop();
   std::cout << "\n Computation with ordinary mutex locking : " << std::endl;
   TM.Report();
 
   delete TR;
   delete TW;
   }


// Auxiliary function
// ------------------
void InputData()
    {
    FILE *fp;
    char buffer[128];

    if( !(fp = fopen("vaccs.dat", "r") ))
	   {
	   cout << "\n Input error" << endl;
	   exit(0);
	   }
	
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &nTh);
    fgets(buffer, 80, fp);
    sscanf(buffer, "%d", &main_wait);
    fclose(fp);
    }
   


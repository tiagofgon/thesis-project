// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// VAccessTbb.C
//
// Uses TBB shared lock to implement thread safety in
// write-read access to a stl::vector<int> container 
// 
// A Timer is used to slow down threads
//
// Main thread uses the BLock B to stop a read loop
// executed by reader threads.
//
// There are TWO THREAD SETS, with nTh threads each: one
// set TR for reader tasks, and one set TW for writer
// tasks. 
// -----------------------------------------------------

#include <tbb/spin_rw_mutex.h>
#include <tbb/mutex.h>
#include <stdio.h>
#include <stdlib.h>
#include <SafeCout.h>
#include <Timer.h>
#include <BLock.h>
#include <SPool.h>
#include <vector>
#include <print_container.h>

void InputData();

using namespace std;

int         nTh, main_wait;
SPool       *TR, *TW;
BLock       B(false);
tbb::mutex  out_mutex;

std::vector<int>     V;
tbb::spin_rw_mutex   my_mutex;
tbb::mutex           outmutex;
//tbb::spin_rw_mutex::scoped_lock my_lock;

// -------------------------------------------------
// Code executed by reader threads
// Reader threads:
// 1 - Wait for 250 ms
// 2 - Read and print the container content to stdin
// -------------------------------------------------
void Reader_Thread(void *P)
   {  
   int n, rank;
   char buff[128];
   Timer T;

   rank = TR->GetRank();
   while( !B.GetState() )
      {
      T.Wait(250); 
      //my_lock.acquire(my_mutex, false);
         {
         tbb::spin_rw_mutex::scoped_lock mylock(my_mutex, false);
         std::vector<int> VV = V;    // this is the read
         // ------------------------
         // writing to stdout
            {
            tbb::mutex::scoped_lock sc(outmutex);
            print_container(VV, " ", 20);
            }
         // ------------------------
         }
      //my_lock.release();
      }
   } 

/* ---------------------------------------------
 * Code executed by writer threads.
 * A rank k thread repeats 5 times the following 
 * steps
 * 1) perform a timed wait randomly selected in 
 *    [0, 2000] miliseconds, to delay the thread
 * 2) Add a new value to the vector container.
 *    The value addad is 10*rank+n
 * -------------------------------------------*/

void Writer_Thread(void *P)
   {
   int n, rank, msec, value;
   Timer T;
   rank = TW->GetRank();
   value = 10*rank;

   for(n=1; n<=5; n++)
      {
      msec = rand()%2000;
      T.Wait(msec);
         {
         tbb::spin_rw_mutex::scoped_lock(my_mutex, true);
         // ------------------------
         value++;
         V.push_back(value);
         // ------------------------
         }
      }
   }


main(int argc, char **argv)
   {
   int n, status;
   Timer T;

   InputData();
	
   // Insert some values in the vector container
   // ------------------------------------------
   for(int n=1; n<=5; ++n) V.push_back(n);

   TR = new SPool(nTh);
   TW = new SPool(nTh);

   // ----------------------------------------------
   // main() launches the reader and writer threads
   // ----------------------------------------------
   TR->Dispatch(Reader_Thread, NULL);
   TW->Dispatch(Writer_Thread, NULL);

   // -----------------------------------------
   T.Wait(main_wait);
   B.SetState(true);  // stop readers
   // ----------------------------------------

   TR->WaitForIdle();
   TW->WaitForIdle();

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
   


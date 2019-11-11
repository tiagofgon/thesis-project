// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// VAccess_P.C
//
// Uses Reader-Writer lock to implement thread safety in
// write-read access to a stl::vector<int> container.
// The RWLock class used here wraps the Pthreads and
// Windows RWLock.
//
// There is NO CPP11 implementation of this class. In this
// environment, the TBB rwlock can be used instead (look
// at VaccessTbb.C) 
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
#include <BLock.h>
#include <SPool.h>
#include <vector>
#include <print_container.h>
#include <stdio.h>
#include <stdlib.h>

void InputData();

using namespace std;

std::vector<int> V;
int    nTh, main_wait;
RWLock RW;
BLock B(false);
pthread_mutex_t outlock;
SPool *TR, *TW;

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
      RW.Lock(false);
      std::vector<int> VV = V;    // this is the read
      // ------------------------
      pthread_mutex_lock(&outlock);  // exclusive access to stdout
      print_container(VV, " ", 20);
      pthread_mutex_unlock(&outlock);
      // ------------------------
      RW.Unlock(false);
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

      RW.Lock(true);
      // ------------------------
      value++;
      V.push_back(value);
      // ------------------------
      RW.Unlock(true);
      }
   }


main(int argc, char **argv)
   {
   int n, status;
   Timer T;

   InputData();
   pthread_mutex_init(&outlock, NULL);
	
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
   


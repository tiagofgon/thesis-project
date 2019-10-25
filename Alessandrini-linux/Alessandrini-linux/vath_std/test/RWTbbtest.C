// **************************************
// Copyright (c) 2015 Victor Alessandrini
// All rights reserved.
// **************************************
/********************************************************
 * RWtest.C
 * This code tries to use the rwLock interface to produce
 * a busy wait for some threads. 
 ********************************************************/

#include <stdlib.h>
#include <iostream>
#include <tbb/spin_rw_mutex.h>
#include <Timer.h>
#include <SPool.h>
#include <errors.h>

using namespace std;

int flag;
tbb::spin_rw_mutex              my_mutex;
tbb::spin_rw_mutex::scoped_lock my_lock;

SPool *TH;

// -----------------------------------------------------------		
// The idea is simple: worker threads will be blocked, reading
// a flag waiting for a change. Since the protecting mutex is
// a RW lock, reads are concurrent.
// -----------------------------------------------------------
void task(void *printstring)
   {
   int myflag;
   Timer tm;
   cout << "inside reader thread" << endl;
   tm.Wait(500);
   do
      {
      tm.Wait(200);
      my_lock.acquire(my_mutex, false);
      myflag = flag;
      my_lock.release();
      }while(myflag==0);
   
   cout << "\nRead finished from waiting thread" << endl; 
   }

int main(int argc, char *argv[])
   {
   int n, status, nThreads;
   Timer T;

   if(argc==2) nThreads = atoi(argv[1]);
   else nThreads = 2;
   flag = 0;

   cout << "\n *** Testing TBB spin reader-writer lock\n " << std::endl;

   TH = new SPool(nThreads);
   TH->Dispatch(task, NULL);
   T.Wait(1000);    // main waits
   cout << "\n Main writes new value of flag " << endl;
   my_lock.acquire(my_mutex, true);
   flag = 1;
   my_lock.release();
   TH->WaitForIdle();
  
   delete TH; 
   return 0;
   }  

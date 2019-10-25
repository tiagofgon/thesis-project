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
#include <errors.h>

#define MAX_TH 16

using namespace std;

int flag;
tbb::spin_rw_mutex              my_mutex;
tbb::spin_rw_mutex::scoped_lock my_lock;
		
void *task(void *printstring)
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
   return NULL;
   }

int main(int argc, char *argv[])
   {
   int n, status, nThreads;
   pthread_t th_id[MAX_TH];
   Timer T;

   if(argc==2) nThreads = atoi(argv[1]);
   else nThreads = 2;
   flag = 0;
   cout << "\n *** Testing TBB spin reader-writer lock\n " << std::endl;

   for(n=0; n<nThreads; n++)
      {
      status = pthread_create(&th_id[n], NULL, task, NULL);
      if(status) error_exit("Thread creation");
      }

   T.Wait(1000);
   cout << "\n Testing TBB reader-writer mutex " << endl;
   cout << "\n Main writes new value of flag " << endl;
   my_lock.acquire(my_mutex, true);
   flag = 1;
   my_lock.release();
   
   for(n==0; n<nThreads; n++)
      {
      status = pthread_join(th_id[n], NULL);
      if(status) error_exit("Thread join");
      }

   return 0;
   }  

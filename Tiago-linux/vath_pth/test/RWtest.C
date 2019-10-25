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
#include <RWLock.h>
#include <Timer.h>
#include <Barrier.h>
#include <errors.h>

#define MAX_TH 16

using namespace std;

int flag;
RWLock my_lock;
Barrier *B;
		
void *task(void *printstring)
   {
   int myflag;
   Timer tm;
   cout << "inside reader thread" << endl;
   tm.Wait(500);
   do
      {
      tm.Wait(200);
      my_lock.Lock(false);
      myflag = flag;
      my_lock.Unlock(false);
      }while(myflag==0);
   tm.Wait(500);
   cout << "\nWaiting thread read flag = " << myflag << endl; 
   tm.Wait(500);
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
   B = new Barrier(nThreads);
   
   cout << "\n Testing vath RWLock class " << endl;

   for(n=0; n<nThreads; n++)
      {
      status = pthread_create(&th_id[n], NULL, task, NULL);
      if(status) error_exit("Thread creation");
      }

   T.Wait(1000);
   cout << "\n Main writes new value of flag " << endl;
   my_lock.Lock(true);
   flag = 1;
   my_lock.Unlock(true);
   
   for(n==0; n<nThreads; n++)
      {
      status = pthread_join(th_id[n], NULL);
      if(status) error_exit("Thread join");
      }

   return 0;
   }  

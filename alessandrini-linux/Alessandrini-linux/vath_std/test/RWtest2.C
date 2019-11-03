// **************************************
// Copyright (c) 2015 Victor Alessandrini
// All rights reserved.
// **************************************
/********************************************************
 * RWtest2.C
 * This code tries to use the rwLock interface to produce
 * a busy wait for some threads. 
 ********************************************************/

#include <stdlib.h>
#include <iostream>
#include <RWLock.h>
#include <Timer.h>
#include <SafeCout.h>
#include <errors.h>

#define MAX_TH 16

using namespace std;

int      flag;
double   x;
RWLock   my_lock;
SafeCout SC;
		
void *task(void *printstring)
   {
   std::ostringstream os;
   int myflag;
   Timer tm;
   cout << "inside reader thread" << endl;
   tm.Wait(500);
   do
      {
      //tm.Wait(200);
      os << x;
      SC.Flush(os);

      my_lock.Lock(false);
      myflag = flag;
      my_lock.Unlock(false);
      }while(myflag==0);
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
   x = 0.2538;

   // Launch threads
   // --------------
   for(n=0; n<nThreads; n++)
      {
      status = pthread_create(&th_id[n], NULL, task, NULL);
      if(status) error_exit("Thread creation");
      }
   cout << "\n *** nother test of Pthreads RWLock class\n" << endl;

   for(int k=0; k<4; k++)
      {
      T.Wait(1000);
      cout << "\n Main writes new value x " << endl;
      my_lock.Lock(true);
      x += 1;
      my_lock.Unlock(true);
      }

   cout << "\n Main switches continuation flag " << endl;
   my_lock.Lock(true);
   flag = true;
   my_lock.Unlock(true);
      
   // Join threads
   // ------------
   for(n==0; n<nThreads; n++)
      {
      status = pthread_join(th_id[n], NULL);
      if(status) error_exit("Thread join");
      }

   return 0;
   }  

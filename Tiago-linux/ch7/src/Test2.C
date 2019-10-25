// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
/*
 * Test2.C
 *
 * Testing the memory consistency model
 * *************************************
 * This code works perfectly well on x86 systems.
 * It deadlocks on IBM P6, which has a relaxed memory model, because
 * thread 1 never gets the memory modification of the main thread,
 *
 * Several things may happen:
 * - the inclrease of synch by the main thread is never flushed to memory
 * - the worker thread keep reading synch from a register and do not access
 *   memory.
 *
 * Same as Test1.C, but we add a mutex lock for changing the 
 * synch value. This does not help. The problem is in the reading.  
 */

#include <stdlib.h>
#include <stdio.h>
#include <SPool.h>

int synch;
SPool TS(1);
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void ThreadFct(void *P)
   {
   while(synch != 1);
   printf("\n Thread has been released\n");
   }

int main(int argc, char **argv)
   {
   synch = 0;

   // launch thread, increase synch and join
   TS.Dispatch(ThreadFct, NULL);
   pthread_mutex_lock(&mutex);
   synch ++;
   pthread_mutex_unlock(&mutex);
   TS.WaitForIdle();
   return 0;
   }





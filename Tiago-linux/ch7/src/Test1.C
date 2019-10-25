// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
/*
 * Test1.C
 *
 * Testing the memory consistency model
 * *************************************
 * This code works perfectly well on Linux x86 systems (32 and 64 bits)
 * It deadlocks on other systems, like the IBM P6, which has a relaxed 
 * memory model, because thread 1 never gets the memory modification 
 * coming from the main thread,
 *
 * Several things may happen:
 * - the increase of synch by the main thread is never flushed to memory
 * - the worker thread loads synch to a register and keeps reading from 
 *   it because it does not have a valid reason for accessig main memory
 *   again (it does not know that somebody else is writing)
 */

#include <stdlib.h>
#include <stdio.h>
#include <SPool.h>

int synch;
SPool TS(1);

void ThreadFct(void *P)
   {
   while(synch != 1);
   printf("\n Thread has been released\n");
   }

int main(int argc, char **argv)
   {
   synch = 0;
   // launch thread, increase synch and join
   // --------------------------------------
   TS.Dispatch(ThreadFct, NULL);
   synch ++;
   TS.WaitForIdle();
   return 0;
   }





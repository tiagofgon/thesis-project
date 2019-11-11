// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// *******************************************
/*
 * Test3.C
 *
 * Testing the memory consistency model
 * *************************************
 * This code works perfectly well on x86 systems (including ulam)
 * It deadlocks on IBM P6, which has a relaxed memory model, because
 * thread 1 never gets the memory modification of the main thread
 *
 * Several things may happen:
 * - the inclrease of synch by the main thread is never flushed to memory
 * - the worker thread keep reading synch from a register and does not 
 *   access memory.
 *
 * This is the same as Test1.C, but now we add a printf function call 
 * inside the while loop. Now this works on IBM P6! This probably 
 * forces the compiler to use the register holding synch to run the 
 * function, and then it has to get the value back from memory when 
 * the function returns.
 */

#include <stdlib.h>
#include <stdio.h>
#include <SPool.h>

int synch;
SPool TS(1);

void ThreadFct(void *P)
   {
   while(synch != 1)
     { printf("\n Reading synch\n"); }
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





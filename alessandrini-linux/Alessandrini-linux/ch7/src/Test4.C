// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ************************************************
//  Test4.C
//  The correct, portable way of releasing a thread
//  from a busy wait
//  -----------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <SPool.h>

int synch;
SPool TS(1);
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void ThreadFct(void *P)
   {
   int my_synch;
   do
      {
      // ---------------------------
      pthread_mutex_lock(&mutex);
      my_synch = synch;
      pthread_mutex_unlock(&mutex);
      // ---------------------------
      }while(synch==0);
   printf("\n Thread has been released\n");
   }

int main(int argc, char **argv)
   {
   synch = 0;

   TS.Dispatch(ThreadFct, NULL); 
   // ----------------------------
   pthread_mutex_lock(&mutex);
   synch ++;
   pthread_mutex_unlock(&mutex);
   // ----------------------------
   TS.WaitForIdle();
   return 0;
   }





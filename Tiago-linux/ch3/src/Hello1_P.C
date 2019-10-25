// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File Hello1_P.C
// 
// Very simple example of launching and joining a
// set of Posix threads.
// ----------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NTHREADS  4
#define ErrExit(A) fprintf(stderr, A); exit(0)

// Array to store the thread identities
// -------------------------------------
pthread_t hello_id[NTHREADS];

/* ---------------------------------------------------
 * This is the thread function executed by the thread.
 * Thread functions receive and return a (void *), but
 * here the argument and return value are ignored
 * -------------------------------------------------*/

void *hello_world (void *arg)
   {
   printf ("Hello IDRIS \n");   
   return NULL;
   }


int main (int argc, char *argv[])
   {
   int i, status;

   /* ----------------------------------------------------------
    * Create threads. The first NULL means that the new thread
    * picks the default attributes. The second NULL is the void
    * pointer passed as argument to the thread function
    * -------------------------------------------------------- */

   for(i=0; i<NTHREADS; i++)
      {   
      status = pthread_create (&hello_id[i], NULL, hello_world, NULL);
      if (status != 0) { ErrExit("Error : Create thread"); }
      }

   /* -----------------------------------------------------------
    * Join the threads in the same order they have been created.
    * The NULL argument means that we do not want to receive the
    * return value of the thread function via the join operation.
    * ---------------------------------------------------------*/

   for(i=0; i<NTHREADS; i++)
      {  
      status = pthread_join (hello_id[i], NULL);
      if (status != 0) { ErrExit("Error : Join thread"); }
      }

   printf("\n From main : threads have been joined\n");  
   return 0;
   }

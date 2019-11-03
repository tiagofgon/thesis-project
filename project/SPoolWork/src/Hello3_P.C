// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File Hello3_P.c
// 
// Very simple example of launching and joining a
// set of threads. Same as Phello2.c, but now we 
// pass an integer rank to each thread from a
// global array. There is no race condition in this
// case
// ----------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NTHREADS  4
#define ErrExit(A) fprintf(stderr, A); exit(0)

/* -----------------------------------------------
 * The first array will hold the thread identities
 * returned by pthread_create.
 * The second array holds the ranks passed to the
 * threads
 * ----------------------------------------------*/

pthread_t hello_id[NTHREADS];
int       th_ranks[NTHREADS];

/* ---------------------------------------------------
 * This is the thread function executed by the thread.
 * Thread functions receive and return a (void *). The
 * return value is ignored. Look how the function
 * argument is used to communicate to each thread its
 * integer rank.
 * -------------------------------------------------*/

void *hello_world (void *arg)
   {
   int rank;
   rank = *(int *)arg;   // cast arg to integer pointer, and read value 
   printf ("Hello IDRIS from thread %d \n", rank);   
   return NULL;
   }


int main (int argc, char *argv[])
   {
   int n, status;

   /* Initialize the ranks array */
   for(n=0; n<NTHREADS; n++) th_ranks[n] = n;

   /* ----------------------------------------------------------
    * Create threads. The NULL second argument means that the new 
    * thread picks the default attributes. The last argument is the 
    * argument that the library will pass to the thread function.
    * -------------------------------------------------------- */

   for(n=0; n<NTHREADS; n++)
      {   
      status = pthread_create (&hello_id[n], NULL, hello_world, 
                               &th_ranks[n]);
      if (status != 0) { ErrExit("Error : Create thread"); }
      }

   /* -----------------------------------------------------------
    * Join the threads in the same order they have been created.
    * The NULL argument means that we do not want to receive the
    * return value of the join operation.
    * ---------------------------------------------------------*/

   for(n=0; n<NTHREADS; n++)
      {  
      status = pthread_join (hello_id[n], NULL);
      if (status != 0) { ErrExit("Error : Join thread"); }
      }

   printf("\n From main : threads have been joined\n");  
   return 0;
   }

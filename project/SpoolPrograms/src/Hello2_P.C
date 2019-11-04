// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File Hello2_P.c
// 
// Very simple example of launching and joining a
// set of threads. We try to pass an integer rank 
// to each thread, as an argument to the thread
// function
//
// WE PASS THE INTEGER RANK IN AN INCORRECT WAY.
// This is the first example of a RACE CONDITION,
// ----------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NTHREADS  4
#define ErrExit(A) fprintf(stderr, A); exit(0)

/* ------------------------------------------
 * This array will hold the thread identities
 * returned by pthread_create.
 * -----------------------------------------*/

pthread_t hello_id[NTHREADS];

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

   /* ----------------------------------------------------------
    * Create threads. The NULL second argument means that the new 
    * thread picks the default attributes. The last argument is the 
    * argument that the library will pass to the thread function.
    * -----------------------------------------------------------
    * We pass directly the address of the loop variable n. This
    * looks correct, but it is not. Threads will not always get
    * the correct value, as it can be checked by executing the
    * program. See comment at the end.
    * -------------------------------------------------------- */

   for(n=0; n<NTHREADS; n++)
      {   
      status = pthread_create (&hello_id[n], NULL, hello_world, &n);
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

/* ---------------------------------------------------------
 * THE RACE CONDITION:
 *
 * The main thread increases the local variable n and passes
 * its address to the new thread.
 *
 * The program makes the incorrect assumption that the new
 * thread will have time to read the value of n BEFORE main
 * has increased it again. This cannot be guaranteed, because
 * threads are asynchronous. 
 *
 * In fact, you will find in executing the program that
 * different threads end up with the same rank value, and
 * that some rank values are missing
 * --------------------------------------------------------*/

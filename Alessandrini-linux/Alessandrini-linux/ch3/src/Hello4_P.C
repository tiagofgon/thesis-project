// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File Hello4_P.c
// ---------------
// Very simple example of launching and joining a
// set of threads. 
//
// Rather than passing an integer value to the thread
// function (equal to the order in which it has been
// created) the same result is obtained by detecting
// the place of the thread ID in the array of worker
// threads IDs. A function GetRank() does this job..
// ----------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NTHREADS  4
#define ErrExit(A) fprintf(stderr, A); exit(0)

/* -----------------------------------------------
 * This array holds the thread identities
 * returned by pthread_create
 * ----------------------------------------------*/

pthread_t hello_id[NTHREADS+1];


// -----------------------------------------
// Auxiliary function, it returns the caller
// thread rank
// 
// This function:
// -) gets the identity of the caller thread
// -) scans the array where the worker thread
//    identities are stored
// -) In this way, it finds its place in the
//    array. Returns the array index
// ------------------------------------------
int GetRank()
   {
   pthread_t my_id;
   int n, my_rank, status;

   my_id = pthread_self();          // determine who  am
   
   n = 0;
   do
      {
      n++;
      status = pthread_equal(my_id, hello_id[n]);
      } while(status==0 && n < NTHREADS); 
 
   if(status) my_rank=n;    // OK, return rank
   else my_rank = (-1);     // else, return error
   return my_rank;
   }


/* ---------------------------------------------------
 * This is the thread function executed by the thread.
 * -------------------------------------------------*/

void *hello_world (void *arg)
   {
   int rank;
   rank = GetRank();
   printf ("Hello world, from thread %d \n", rank);   
   return NULL;
   }


int main (int argc, char *argv[])
   {
   int n, status;

   /* ----------------------------------------------------------
    * Create threads. The NULL second argument means that the new 
    * thread picks the default attributes. The last argument is the 
    * argument that the library will pass to the thread function.
    * -------------------------------------------------------- */

   for(n=1; n<=NTHREADS; n++)
      {   
      status = pthread_create (&hello_id[n], NULL, hello_world, 
                               NULL);
      if (status != 0) { ErrExit("Error : Create thread"); }
      }

   /* -----------------------------------------------------------
    * Join the threads in the same order they have been created.
    * The NULL argument means that we do not want to receive the
    * return value of the join operation.
    * ---------------------------------------------------------*/

   for(n=1; n<=NTHREADS; n++)
      {  
      status = pthread_join (hello_id[n], NULL);
      if (status != 0) { ErrExit("Error : Join thread"); }
      }

   printf("\n From main : threads have been joined\n");  
   return 0;
   }

// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
/* -------------------------------------------------------
 * This file is RetString_P.c
 * The purpose of this code is to demontrate how a process
 * that launches a thread, can recover a return value when
 * the thread finishes. In this particular case, the calling
 * process receives a C string (in fact, its address)
 * -------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#define ErrExit(A) fprintf(stderr, A); exit(0)

pthread_t my_thread;

void *thread_fct (void *arg)
   { 
   /* -------------------------------------------
    * This thread function:
    * -) allocates heap memory for a string
    * -) initializes the string 
    * -) returns its address 
    * ------------------------------------------*/

   char *p;
   p = (char *)malloc(64*sizeof(char));
   strcpy(p, "\nThis is the string initialized by the worker thread\n");
   return (void *)p;
   }


int main (int argc, char *argv[])
   {
   int status;
   void *P;    /* this pointer receives the thread return value 
	        * we will pass ITS ADDRESS to pthread_join()*/

   /* -------------
    * Create thread 
    * -------------*/
      
   status = pthread_create (&my_thread, NULL, thread_fct, NULL);
   if (status != 0) { ErrExit("Error : Create thread"); }

    /* ----------------------------------------------------
     * Join the thread, recover the return value, print the
     * string, release memory allocated by thread 
     * ---------------------------------------------------*/

   status = pthread_join (my_thread, &P);
   if (status != 0) { ErrExit("Error : Join thread"); }
   printf("%s", (char *)P);
   free(P);
  
   return 0;
   }

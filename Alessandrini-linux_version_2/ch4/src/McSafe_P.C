// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ***********************************************
// File McSafe_P.C
// 
// MonteCarlo computation of PI
//
// Thread safe version, using the Pthreads KEY thread
// local storage service. Notice that the operation of
// this service is very similar to the TBB or Windows
// implementations: a global container of pointers, that
// stores the pointers owned by different threads.
// -----------------------------------------------

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <SPool.h>
#include <errors.h>
#include <CpuTimer.h>

using namespace std;


// *****< Construction of a thread safe generator >********

void InitRandk(long seed);     // forwars declarations
double Randk();

// Constants used by generator
// ---------------------------

int  IMUL = 314159269;
int  IADD = 453806245;
int  MASK = 2147483647;
double SCALE = 0.4656612873e-9;

// Key data structures, needed to implement the thread
// local storage service: "seed_key" is the pointer
// container, and init_once is a data item needed for
// the unique creation of "seed_key" by one of the
// worker threads  
// ----------------------------------------------------

pthread_key_t seed_key;      
pthread_once_t init_once = PTHREAD_ONCE_INIT;


// This first function will be called, via pthread_once
// by one of the worker threads to perform the initialization 
// of seed_key
// ----------------------------------------------------------
void once_routine(void)
   {
   int status;
   printf("Initializing key\n");
   status = pthread_key_create(&seed_key, NULL);
   if(status != 0) err_abort(status, "Create Key");
   }

// ---------------------------------------------------------
// This function will initialize the random number generator
// Uses pthread_once to filter the seed_key initialization by
// one thread: only the first caller executes the once_routine.
// This function also creates and initializes the pointer owned
// each one of the worker threads
// -----------------------------------------------------------
void InitRandk(long seed)
    {
    int *seedptr;
    int status;
             
    // Key creation with pthread_once:: only the first caller
    // executes once_routine, pthread_once returns immedialtely
    // for the next callers
    // -------------------------------------------------------
    status = pthread_once (&init_once, once_routine);
    if (status != 0) err_abort (status, "Once init");
    
    // Allocate memory in the heap for the seed. Every thread will 
    // have of course its own particular pointer to long in the heap.
    // -------------------------------------------------------------
    seedptr = (int *)malloc (sizeof (int));
    if (seedptr == NULL) errno_abort ("Allocate key value");
    *seedptr = seed;

    // seedptr is a local variable: it will evaporate when this function 
    // returns. But we can keep a permanent copy of this pointer in the
    // seed_key container, as follows
    // ---------------------------------------------------------------
    status = pthread_setspecific(seed_key, (void *)seedptr);
    if(status != 0) err_abort(status, "Initial SetSpecific");
    }
    
// ---------------------------------------------
// Now, the thread safe randon number generator
// ---------------------------------------------

double Randk()
  {
  int *myseed;
  int status;
  
  // Retrieve the pointer owned by thos thread
  // -----------------------------------------
  myseed = (int *)pthread_getspecific(seed_key);

  // Operate as ussual
  // -----------------
  *myseed = (*myseed * IMUL + IADD) & MASK;
  return (*myseed * SCALE);
  }

// *************************************************


// Global variables for this example
// ---------------------------------
int  nTh;
SPool TH(2);           // pool of two threads
long nsamples;         // number of MC events
long accepted[3];      // storage of partial results

// The task function
// -----------------
void task_fct(void *P)
   {
   double x, y;
   long count;
   long my_seed; 
   int rank;

   rank = TH.GetRank();   // in SPool, ranks start at 1
   my_seed = 999*rank;    // thread dependent initialization
   InitRandk(my_seed);

   count = 0;
   for(size_t n=0; n<nsamples; n++)
      {
      x = Randk();
      y = Randk();
      if((x*x+y*y) <= 1.0 ) count++;
      }
   accepted[rank] = count;
   }

// The main function
// -----------------
int main(int argc, char **argv)
   {
   long C;
   CpuTimer T;

   if(argc==2) nTh = atoi(argv[1]);
   else nTh = 2;
   nsamples = 1000000000;

   T.Start();
   TH.Dispatch(task_fct, NULL);
   TH.WaitForIdle();
   T.Stop();

   C = accepted[1]+accepted[2];
   double pi = 2.0 * (double)C / nsamples;
   cout << "\n Value of PI = " << pi << endl;
   T.Report();
   }

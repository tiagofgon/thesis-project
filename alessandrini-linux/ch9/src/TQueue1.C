// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File TQueue1.C
// ------------------
//
// This example deals with three producer threads,
// and one consumer thread. All producers keep inserting
// integers until one of them closes the queue. In this 
// example, the basic mechanism for termination is checked: 
// the remaining producers stop inserting, and the consumer 
// thread correctly drains the queue extracting all the
// residual integers inserted in the queue. 
//
// A pool of three producer threads is created. The main thread, 
// rather than doing nothing, will be the consumer, extracting 
// integer values and printing them in the screen.
//
// - Producer 1 inserts 100 integers, starting from 1. Then,
//   it closes the queue
// - Producer 2 inserts consecutive integers starting from 
//   1000, until the queue is closed.
// - Idem for producer 3, starting at 100000.
//
// In this way, it is easy to identify which producer 
// inserted the integers printed by main
// ----------------------------------------------

#include <iostream>
#include <ThQueue.h>
#include <SPool.h>
#include <Timer.h>

int N;               // working buffer size
int C;               // queue capacity
SPool  TS(3);        // three worker threads, the producers
ThQueue<int> *THQ;   // reference to thread safe queue

void ThFct(void *arg)       // Thread function
   {
   int n, start_index, retval;
   int rank = TS.GetRank();

   if(rank==1)    // Producer 1 code       
      {
      for(n=1; n<=100; n++) THQ->Add(n);
      THQ->CloseQueue();
      }
   else          // Producers 2 and 3 code
      {
      if(rank==2) start_index = 1000;
      else start_index = 100000;
      n = 1;
      do
         {
         retval = THQ->Add(start_index+n);
         n++;
         }while(retval);
      } 
   }

int main(int argc, char **argv)
   {
   bool read_flag;
   int  read_value;

   if(argc==2) C = atoi(argv[1]);
   else C = 40;
   THQ = new ThQueue<int>(C);
   
   TS.Dispatch(ThFct, NULL);    // run threads
   // ------------------------------------------
   // This main thread dequeues and print values
   // as long as they are relevant
   // ------------------------------------------
   for(;;)
      {
      read_value = THQ->Remove(read_flag);
      if(read_flag == false) break;
      else std::cout << read_value << std::endl;
      }
   TS.WaitForIdle();   // synchronize with workers
   delete THQ;
   } 

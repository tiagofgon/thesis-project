// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File TQueue2.C
// ------------------

// This example deals with one producer and three consumer
// threads. The producers inserts integers and then closes 
// the queue. In this example, the basic mechanism for 
// termination is checked again: the consumer threads 
// correctly drain the queue extracting all the integers 
// inserted in the queue. 
//
// A pool of three consumer threads is created. The main thread, 
// rather than doing nothing, will be the producer, inserting 
// integer values.
//
// - Producer inserts 200 integers, starting from 1. Then,
//   it closes the queue.
// ----------------------------------------------

#include <iostream>
#include <sstream>
#include <ThQueue.h>
#include <SPool.h>
#include <Timer.h>
#include <SafeCout.h>

int N;               // working buffer size
int C;               // queue capacity
SPool        TS(3);  // three worker threads, the producers
ThQueue<int> *THQ;   // reference to thread safe queue
SafeCout     SC;

void ThFct(void *arg)       // Thread function
   {
   int read_value;
   bool read_flag; 
   std::ostringstream os;

   for(;;)
      {
      read_value = THQ->Remove(read_flag);
      if(read_flag == false) break;
      else 
         {
         os << read_value;
         SC.Flush(os);
         }
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
   for(int n=1; n<=200; n++) THQ->Add(n);  // insert in queue
   THQ->CloseQueue();                  // close queue
   // ----------------------------------------
   TS.WaitForIdle();   // synchronize with workers
   delete THQ;
   } 

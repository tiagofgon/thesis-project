// =======================================
// Copyright (c) 2012 Victor Alessandrini
// All rights reserved.
// =======================================
// File FlowControl.C
// ------------------

// The purpose of this example is to test the
// ThQueue<T> software
// ----------------------------------------------

#include <iostream>
#include <ThQueue.h>
#include <Timer.h>
#include <SPool.h>
#include <stdlib.h>

using namespace std;

int N;               // working buffer size
int C;               // queue capacity
SPool  TS(1);        // one worker thread, main thread is producer
ThQueue<int> *THQ;   // reference to thread safe queue

void ThFct(void *arg)       // Thread function
   {
   int n, index; 
   double d = 0.0;
   int rank = TS.GetRank();

   // Consumer code
   // Producer has inserted four integer values in the queue
   // -------------
   bool flag;
   Timer T;
   do
     {
     index = THQ->Remove(flag);
     if(flag) cout << "\n Removed value " << index  << endl;
     else cout << "\n Remove returned fake value " << endl;
     T.Wait(1000);
     }while(flag);
   }


int main(int argc, char **argv)
   {
   int n;

   if(argc==2) C = atoi(argv[1]);
   else C = 8;
   std::cout << "\n *** First test of ThQueue class\n " << std::endl;

   // Allocations
   // -----------
   THQ = new ThQueue<int>(C);
   
   // Insert in queue
   // ---------------
   for(n=1; n<5; n++)
     THQ->Add(n);

   // Run threads
   // -----------
   
   TS.Dispatch(ThFct, NULL);   // launches worker thread
   for(n=1; n<5; n++) THQ->Add(n);   // insert in queue
   cout << "\n Producer closing queue..." << endl;
   THQ->CloseQueue();
   TS.WaitForIdle();

   delete THQ;
   } 

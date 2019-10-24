// =======================================
// Copyright (c) 2012 Victor Alessandrini
// All rights reserved.
// =======================================
// File FlowControl.C
// ------------------

// The purpose of this example is to show how the
// ThQueue<T> capacity can be used to synchronize
// producers and consumers threads
// ----------------------------------------------

#include <iostream>
#include <ThQueue.h>
#include <SPool.h>
#include <Timer.h>

int N;               // working buffer size
int C;               // queue capacity
SPool  TS(2);        // two worker threads
ThQueue<int> *THQ;   // reference to thread safe queue

void ThFct(void *arg)       // Thread function
   {
   int n, index; ;
   int rank = TS.GetRank();

   // Producer code
   // -------------
   if(rank==1)       
      {
      for(n=0; n<50; n++) THQ->Add(n);
      THQ->CloseQueue();
      }

   // Consumer code
   // -------------
   if(rank==2)    
      {
      bool flag;
      Timer T;
      index = THQ->Remove(flag);
      while(flag)
         {
         std:: cout << index << std::endl;
         T.Wait(200);
         index = THQ->Remove(flag);
         }
      }
   }

int main(int argc, char **argv)
   {
   int nc;
   cout << "Enter queue size:" << endl;
   cin >> C;

   THQ = new ThQueue<int>(C);
   std::cout << "\n *** Second test of ThQueue class \n " << std::endl;

   // Run threads
   // -----------
   TS.Dispatch(ThFct, NULL);
   TS.WaitForIdle();

   delete THQ;
   } 

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
double *wkbuffer ;   // reference to working buffer

void ThFct(void *arg)       // Thread function
   {
   int n, index; 
   double d = 0.0;
   int rank = TS.GetRank();

   // Producer code
   // -------------
   if(rank==1)       
      {
      for(n=0; n<50; n++)
         {
         d += 0.25;
         index = n%N;
         wkbuffer[index] = d;
         THQ->Add(index);
         }
      //index = -1;         // signal end of job
      // THQ->Add(index);
      THQ->CloseQueue();
      }

   // Consumer code
   // -------------
   if(rank==2)    
      {
      bool flag;
      Timer T;
      index = THQ->Remove(flag);
      //while(index>=0)
      while(flag)
         {
         std:: cout << wkbuffer[index] << std::endl;
         T.Wait(200);
         index = THQ->Remove(flag);
         }
      }
   }

int main(int argc, char **argv)
   {
   int nc;
   cout << "\n Testing ThQueue capacity\n" << std::endl;

   N = 10;
   C = 10;
   cout << "Current parameters are buffer size : " << N << "   capacity: "
        << C << endl;

   // Allocations
   // -----------
   THQ = new ThQueue<int>(C);
   wkbuffer = new double[N];

   // Run threads
   // -----------
   TS.Dispatch(ThFct, NULL);
   TS.WaitForIdle();

   delete THQ;
   delete [] wkbuffer;
   } 

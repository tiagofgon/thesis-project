// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File TRBuff.C
// 
// Testing the RingBuffer class for integers. Producer 
// inserts consecutive integer values, consumer reads
// and prints them. The buffer size is a template 
// parameter
// ------------------------------------------------
#include <SPool.h>
#include <iostream>
#include <RingBuffer.h>

RingBuffer<int, 22> R;   // integer buffer, size = 22
SPool *TP;

void thread1()
   {
   int n;
   for(n=1; n<=100; ++n) R.Insert(n);
   std::cout << "\n Provider thread finished " << std::endl;
   }

void thread2()
   {
   int value;
   std::cout << std::endl;
   do
      {
      R.Extract(value);
      std::cout << value <<std::endl;
      } while (value != 100);
   std::cout << "\n Consumer thread finished " << std::endl;
   }


void th_fct(void *p)
   {
   int rank = TP->GetRank();
   switch(rank)
      {
      case 1: thread1();
              break;
      case 2: thread2();
              break;
      }
   }

int main(int argc, char **argv)
   {
   TP = new SPool(2);
   TP->Dispatch(th_fct, NULL);
   TP->WaitForIdle();
   delete TP;
   }

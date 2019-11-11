// **************************************
// Copyright (c) 2012 Victor Alessandrini
// All rights reserved.
// **************************************
// File TRingBuff.C
// --------------------------------------
// Testing the RingBuffer class. Producer
// inserts, consumer reads
// --------------------------------------
#include <iostream>
#include <RingBufferSC.h>
#include <SPool.h>

SPool   *SP;
RingBufferSC<int, 22> R;

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

void task_fct(void *P)
   {
	int rank = SP->GetRank();
	if (rank == 1) thread1();
	if (rank == 2) thread2();
    }

int main(int argc, char **argv)
   {
   std::cout << "\n *** Testing the RingBuffer class\n " << std::endl;

   SP = new SPool(2);
   SP->Dispatch(task_fct, NULL);
   SP->WaitForIdle();
   std::cout << "\n Parallel job finished" << std::endl;
   delete SP;
   }

// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File NPool1.C
// -------------
// Tests the very general features of the NPool facility,
// including job status information. 
// ------------------------------------------------------

#include <stdlib.h>
#include <iostream>
#include <NPool.hpp>
#include <Timer.hpp>
#include <RandInt.hpp>
#include <SafeCout.hpp>
#include <SafeCounter.hpp>
#include <CpuTimer.hpp>
#include <atomic>
#include <memory>

#include <future>


using namespace std;

NPool *NP;

int fib(int num){
   if(num<2) {
     return num;
   }
   else {
      int a, b;
         Task *t1 = new Task();
         Task *t2 = new Task();

         auto future1 = t1->insertTask(fib, num-1);
         auto future2 = t2->insertTask(fib, num-2);
         
         NP->SpawnTask(t1, true);
         NP->SpawnTask(t2, true);
         NP->TaskWait();
         a = future1.get();
         b = future2.get();

         return a+b;
      }
   }

int main(int argc, char *argv[]) {
   if(argc != 2) {
      std::cout << "Please put one argumet as integer to calculate fibonacci" << std::endl;
      return 1;
   }
  
   bool status;
   int taskID;
   int nthreads=4;
   CpuTimer TR;   
   int numero(atoi(argv[1]));

   NP = new NPool(nthreads);
   
   TaskGroup *TG = new TaskGroup();
   Task *t = new Task();

   auto future = t->insertTask(fib, numero);
   

   TG->Attach(t);


   TR.Start();
   
   taskID = NP->SubmitJob(TG);
   
   NP->WaitForIdle();
   TR.Stop();

   TR.Report();
   int soma = future.get();
   std::cout << "O resultado é: " << soma << std::endl;
  
   return 0;
}  

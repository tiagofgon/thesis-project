// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File TaskCentricPool.C
// -------------
// Tests the very general features of the TaskCentricPool facility,
// including job status information. 
// ------------------------------------------------------

#include <stdlib.h>
#include <iostream>
#include <TaskCentricPool.hpp>
#include <Timer.hpp>
#include <RandInt.hpp>
#include <SafeCout.hpp>
#include <SafeCounter.hpp>
#include <CpuTimer.hpp>
#include <atomic>
#include <memory>

#include <future>


using namespace std;

TaskCentricPool *NP;



void fib(int num, int *soma){
   if(num<2) {
     *soma=num;
   }
   else {
      int a, b;
         Task *t1 = new Task();
         Task *t2 = new Task();

         t1->insertTask(fib, num-1, &a);
         t2->insertTask(fib, num-2, &b);
         
         NP->SpawnTask(t1, true);
         NP->SpawnTask(t2, true);

         NP->TaskWait();
         *soma = a+b;

         //std::cout << "ola_1" << std::endl;

         
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

   NP = new TaskCentricPool(nthreads);
   
   TaskGroup *TG = new TaskGroup();
   Task *t = new Task();
   int soma=0;
   t->insertTask(fib, numero, &soma);


   TG->Attach(t);


   TR.Start();
   
   taskID = NP->SubmitJob(TG);
   
   NP->WaitForIdle();
   TR.Stop();

   TR.Report();

   std::cout << "a soma é: " << soma << std::endl;
  
   return 0;
}  

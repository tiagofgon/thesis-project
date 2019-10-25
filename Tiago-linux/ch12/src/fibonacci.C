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
#include <NPool.h>
#include <Timer.h>
#include <RandInt.h>
#include <SafeCout.h>
#include <SafeCounter.h>
#include <CpuTimer.h>
#include <atomic>
#include <memory>


using namespace std;

NPool *NP;


int fibonacci(int n){
      if(n<2)
         return n;
      else
      {
         return fibonacci(n-1) + fibonacci(n-2);
      }
}

class TestTask: public Task {
   private:
    int rank;
    int num;
    int *sum;

   public:
   TestTask(int r, int rn, int *soma): Task(), rank(r), num(rn), sum(soma) {}

   void ExecuteTask(){
      if(num<2) {
         *sum = num;
      }
      else {
         int a, b;
         TestTask *t1 = new TestTask(rank, num-1, &a);
         TestTask *t2 = new TestTask(rank, num-2, &b);
         
         NP->SpawnTask(t1, true);
         NP->SpawnTask(t2, true);

         NP->TaskWait();
         *sum = a+b;

      }
      
      
      
   }
};



int main(int argc, char *argv[]) {
  
   bool status;
   int taskID;
   int nthreads=4;
   //CpuTimer TR;   
   int result=0;

   int numero(atoi(argv[1]));

   NP = new NPool(nthreads);

   TaskGroup *TG = new TaskGroup();

   TestTask *t = new TestTask(1, numero, &result);
   //TR.Start();
   taskID = NP->SubmitJob(t);
   
   NP->WaitForIdle();
   //TR.Stop();

   cout << "\n Main terminates, result = " << result << endl;
  
   return 0;
}  

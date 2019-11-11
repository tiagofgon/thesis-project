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

atomic<int> result;

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
   //  Timer T;
   //  ostringstream os;

   public:
   TestTask(int r, int rn): Task(), rank(r), num(rn) {}

   void ExecuteTask(){
      if(num<2) {
         result.fetch_add(num);
      }
      else {
         TestTask *t1 = new TestTask(rank, num-1);
         TestTask *t2 = new TestTask(rank, num-2);
         
         NP->SpawnTask(t1, false);
         NP->SpawnTask(t2, false);

         //NP->TaskWait();
      }
      
      
      
   }
};



int main(int argc, char *argv[]) {
  
   bool status;
   int taskID;
   int nthreads=4;
   CpuTimer TR;   
   int numero(atoi(argv[1]));

   NP = new NPool(nthreads);

   TaskGroup *TG = new TaskGroup();

   TestTask *t = new TestTask(1, numero);
   TR.Start();
   taskID = NP->SubmitJob(t);
   
   NP->WaitForIdle();
   TR.Stop();

   TR.Report();
   cout << result.load() << endl;

   //cout << fibonacci(numero) << endl;
  
   return 0;
}  

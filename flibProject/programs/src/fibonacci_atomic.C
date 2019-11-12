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

#include <future>


using namespace std;

NPool *NP;

//atomic<int> result;

// int fibonacci(int n){
//       if(n<2)
//          return n;
//       else
//       {
//          return fibonacci(n-1) + fibonacci(n-2);
//       }
// }

// class TestTask: public Task {
//    private:
//     int rank;
//     int num;
//    //  Timer T;
//    //  ostringstream os;

//    public:
//    TestTask(int r, int rn): Task(), rank(r), num(rn) {}

//    void ExecuteTask(){
//       if(num<2) {
//          result.fetch_add(num);
//       }
//       else {
//          TestTask *t1 = new TestTask(rank, num-1);
//          TestTask *t2 = new TestTask(rank, num-2);
         
//          NP->SpawnTask(t1, false);
//          NP->SpawnTask(t2, false);

//          //NP->TaskWait();
//       }
//    }
// };

auto fib(int num){
      if(num<2) {
         std::cout << "numero recebido= " << num << std::endl;
         return num;
      }
      else {
         std::cout << "numero recebido= " << num << std::endl;
         Task *t1 = new Task();
         Task *t2 = new Task();
         auto future1 = t1->insertTask(fib, num-1);
         auto future2 = t2->insertTask(fib, num-2);
         
         int a = NP->SpawnTask(t1, true);
         int b = NP->SpawnTask(t2, true);
         //std::cout << "a: " << a << "b: " << b << std::endl;
         return future1.get()+future2.get();
         
         //NP->TaskWait();
      }
   }

auto teste(int num){
   if(num==1)
      return 1;
   else {
         Task *t1 = new Task();

         auto future1 = t1->insertTask(teste, num-1);
         
         int a = NP->SpawnTask(t1, true);
         //std::cout << "a: " << a << "b: " << b << std::endl;
         return future1.get()+1;
         
         //NP->TaskWait();
      }
   }

int main(int argc, char *argv[]) {
  
   bool status;
   int taskID;
   int nthreads=4;
   CpuTimer TR;   
   int numero(atoi(argv[1]));

   NP = new NPool(nthreads);
   
   TaskGroup *TG = new TaskGroup();
   // Task *t = new Task();
   // auto result = t->insertTask(fib, numero);
   Task *t = new Task();
   auto result = t->insertTask(teste, numero);
   
   TG->Attach(t);


   TR.Start();
   
   taskID = NP->SubmitJob(TG);
   
   NP->WaitForIdle();
   TR.Stop();

   TR.Report();
   cout << result.get() << endl;
  
   return 0;
}  

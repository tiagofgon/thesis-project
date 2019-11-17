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

#include <future>


using namespace std;

NPool *NP;

// -----------------------------------------
// Auxiliary function, to print a job status
// -----------------------------------------
void ReportStatus(bool S)
   { 
   if(S) cout << "\n Job is queued or running" << endl;
   else cout << "\n Job is finished" << endl;
   }


class SQUARE
{
public:
  auto operator()(const uint64_t x, uint64_t y) {
        return x*x+y;
    };  
};


auto squareT (const uint64_t x, uint64_t y) {
      cout << "olaaaaaaaaaaaaaaaaaaa!" << endl;
        return x*x+y;
    };


int main(int argc, char *argv[]) {
  
  int n;
  bool status;
  int taskID[6];
  int jobID1;
  int jobID2;
  int jobID3;

  NPool NP2(31);

   std::vector<std::future<uint64_t>> futures;

  auto square = [](const uint64_t x, uint64_t y) {
        return x*x+y;
  };

  //NP = new NPool(31);
  TaskGroup *TG1 = new TaskGroup();
  // TaskGroup *TG2 = new TaskGroup();
  // TaskGroup *TG3 = new TaskGroup();
  
  for (n=0; n<5; n++) {
    Task *t = new Task();
    auto future = t->insertTask(square, n, 12);
    //TestTask tt(n);
    auto x = SQUARE();
    //auto future = tt.enqueue(x, 23, 12);
    //auto future = t->enqueue(x, 23, 12);

    futures.emplace_back(std::move(future));
    TG1->Attach(t);
  }


  // for (n=0; n<10; n++) {
  //   TestTask *t = new TestTask(n);
  //   TG2->Attach(t);
  // }
  // for (n=0; n<15; n++) {
  //   TestTask *t = new TestTask(n);
  //   TG3->Attach(t);
  // }


  jobID1 = NP2.SubmitJob(TG1);
  //jobID2 = NP->SubmitJob(TG2);
  //jobID3 = NP->SubmitJob(TG3);
  
  // Wait for parallel jor termination
  // ---------------------------------
  NP2.WaitForJob(jobID1);
  //NP->WaitForJob(jobID2);
  //NP->WaitForJob(jobID3);

  for (auto& future : futures)
        std::cout << future.get() << std::endl;
  
  
}  

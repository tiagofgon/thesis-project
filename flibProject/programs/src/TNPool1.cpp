/* ---------------------------------------------------------------------------
                           Made by Tiago Gonçalves - 2019
 --------------------------------------------------------------------------*/

// File NPool1.cpp
// -------------

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

int main(int argc, char *argv[]) {
  
  int n;
  bool status;
  int taskID[6];
  int jobID1;

  NPool NP2(31);

   std::vector<std::future<uint64_t>> futures;

  auto square = [](const uint64_t x, uint64_t y) {
        return x*x+y;
  };

  TaskGroup *TG1 = new TaskGroup();
  
  for (n=0; n<10; n++) {
    Task *t = new Task();
    auto future = t->insertTask(square, n, 12);

    futures.emplace_back(std::move(future));
    TG1->Attach(t);
  }


  jobID1 = NP2.SubmitJob(TG1);


  // Wait for parallel jor termination
  // ---------------------------------
  NP2.WaitForJob(jobID1);

  for (auto& future : futures)
        std::cout << future.get() << std::endl;
  
  
}  

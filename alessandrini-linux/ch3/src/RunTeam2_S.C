// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// RunTeam2_S.C
//
// Another way of launching a team of worker threads.
// Threads are launched and inserted into a
// std::vector<std::thread> container. 
// -------------------------------------------------

#include <iostream>
#include <thread>
#include <chrono>
#include <vector>

void helloFunc(unsigned rank)
   {
   // create a time duration object and wait
   // -----------------------------------------
   std::chrono::seconds workTime(rank);
   std::this_thread::sleep_for(workTime);
   std::cout << "Hello from thread number " << rank 
             << std::endl;
   }

int main(int argc, char* argv[])
   {
   unsigned n, nTh;
   if(argc==2) nTh = atoi(argv[1]);
   else nTh = 4;

   // ------------------------------------------------------
   // Threads are created and inserted in a vector container
   // There is a lot of intelligence in the two lines below.
   // Normally, STL insertions copy objects. But in this case,
   // the thread objects are not copied, they are MOVED: the
   // objects inside the container get the identity of the
   // inserted objects, and the inserted objects become "not-a
   // thread" objects before dissappearing. See the ThMove_S.C
   // example
   // --------------------------------------------------------
   std::vector<std::thread> workers;
   for (n=0; n<nTh; ++n)
       workers.push_back(std::thread(helloFunc, n));

   for(auto &th : workers) th.join();
   std::cout << "Main: done" << std::endl;
   return 0;
   }



// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// ThMove_S.C
// 
// This example shows how thread ownership is moved across 
// thread objects
// Thread T1 is launched and its id printed Then, thread T1 
// is moved to thread T2, and their ids are printed. T2 has 
// the same identity that T1 had before, and T1 is now a 
// "not a thread" object
// --------------------------------------

#include <iostream>
#include <thread>

void ThreadFunc()
   {   // create a 5 seconds duration object
   // --------------------------------------
   std::chrono::seconds workTime(5);
   std::this_thread::sleep_for(workTime);  // sleep
   }

int main(int argc, char* argv[])
   {

   std::thread T1(ThreadFunc);      // launch thread  T1
   std::cout << "First thread identity = " << T1.get_id() << std::endl;
   std::thread T2 = std::move(T1);  // transfers thread ownership
   std::cout << "Old thread object identity = " << T1.get_id() << std::endl;
   std::cout << "NEW thread object identity = " << T2.get_id() << std::endl;
   T2.join();
   return 0;
   }



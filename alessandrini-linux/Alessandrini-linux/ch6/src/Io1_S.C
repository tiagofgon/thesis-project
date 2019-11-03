// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ***************************************** 
// File Io1_S.C
//
// Demonstrates future synchronization.
// Main launches an asynchtonous task and waits for 
// its future termination.
// -----------------------------------------------
#include <thread>
#include <iostream>
#include <future>
#include <chrono>

// -------------------
// Worker thread code
// -------------------
void io_thread()
   {
   std::chrono::milliseconds ms(4000);
   std::cout << "\n IO thread waiting for four seconds" << std::endl;
   std::this_thread::sleep_for(ms);
   std::cout << "\n IO operation done" << std::endl;
   }   

int main(int argc, char **argv)
   {
   // Dispatch the asynch function
   // ----------------------------
   std::future<void> FT = std::async(io_thread);

   // We immediately wait for the future event
   // ----------------------------------------
   FT.get();
   std::cout << "\n Main terminates" << std::endl;
   }

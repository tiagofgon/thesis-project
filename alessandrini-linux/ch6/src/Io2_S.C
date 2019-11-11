// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ***************************************** 
// File Io2_S.C
//
// Demonstrates synchronization of an IOtask, using future
// TIMED waits. 
//
// Now, main performs successive 1 second timed waits for the 
// future event (the termination of the IO task)
// ---------------------------------------------------------
#include <thread>
#include <iostream>
#include <future>
#include <chrono>

// ----------------------------------
// Worker thread code (same as before)
// ----------------------------------
void io_thread() 
   {
   std::chrono::milliseconds ms(5000);
   std::cout << "\n IO thread waiting for five seconds" << std::endl;
   std::this_thread::sleep_for(ms);
   std::cout << "\n IO operation done" << std::endl;
   }   

int main(int argc, char **argv)
    {
    std::chrono::milliseconds ms(1000);

    // --------------------------------------------
    // Dispatch the asynch function, to perform the
    // asynchronous exacution - in a separate thread -
    // of the io_thread function
    // --------------------------------------------
    std::future<void> FT = std::async(io_thread);

    // Start a do loop involving timed waits
    // -------------------------------------
    std::future_status status;
    do
        {
        std::cout << "\n Main waits for 1 second" << std::endl;
        status = FT.wait_for(ms);
        } while (status == std::future_status::timeout);
    std::cout << "\n Main terminates" << std::endl;
    }

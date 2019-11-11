// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ***************************************** 
// File Pio1_S.C
//
// Demonstrates future synchronization of an IOtask
//
// Rather than dispatching an async function, a new thread is
// dispatched. This new thread will return an integer to main.
//
// A promise<int> object for is used for synchronization
// - The IO task sets the value of the promise when it terminates
// - Main gets a future object from the promise, and when the time
//   comes waits for the future event.
// ------------------------------------------------------------
#include <thread>
#include <iostream>
#include <future>
#include <chrono>

std::promise<int> P;

// -------------------
// Worker thread code
// -------------------
void io_thread()
   {
   std::chrono::milliseconds ms(4000);
   std::cout << "\n IO thread waiting for four seconds" << std::endl;
   std::this_thread::sleep_for(ms);
   std::cout << "\n IO operation done" << std::endl;
   P.set_value(13);        // this makes the associated future ready
   }   

int main(int argc, char **argv)
    {
    // Get the future associated to the promise:
    // -----------------------------------------
    std::future<int> FT = P.get_future();

    // Launch the worker thread and detach it (no need to join)
    // -------------------------------------------------------
    std::thread T(io_thread);
    T.detach();

    // We immediately wait for the future event
    // ---------------------------------------
    int retval = FT.get();
    std::cout << "\n Main terminates with value " 
              << retval << std::endl;
    }

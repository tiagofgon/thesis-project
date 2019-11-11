// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ***************************************** 
// File Pio2_S.C
//
// Same structure as Pio1_S.C. But now, main() waits for
// for the future avent by a succession of 1 second timed waits
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
   std::chrono::milliseconds ms(5000);
   std::cout << "\n IO thread waiting for five seconds" << std::endl;
   std::this_thread::sleep_for(ms);
   std::cout << "\n IO operation done" << std::endl;
   P.set_value(13);        // this makes the associated future ready
   }   

int main(int argc, char **argv)
    {
    // Get the future associated to the promise:
    std::future<int> FT = P.get_future();

    // Launch the worker thread
    std::thread T(io_thread);
    T.detach();

    // Perform 1 second timed waits
    // We start a do loop involving timed waits
    // ---------------------------------------
    std::chrono::milliseconds ms(5000); 
    std::future_status status;
    do
        {
        std::cout << "\n Main waits for 1 second" << std::endl;
        status = FT.wait_for(ms);
        } while (status == std::future_status::timeout);

    // Get the future return value
    // ---------------------------
    int retval = FT.get();
    std::cout << "\n Main terminates with value " << retval << std::endl;
    }

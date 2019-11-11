// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// *******************************************
// ExFuture_S.C
//
// This is a first simple example of getting a return value from
// an asynchronous function call usinf a future<T> object
// --------------------------------------------------------------

#include <future>
#include <iostream>
#include <chrono>

// This is the function that will be executed asynchronously
// ---------------------------------------------------------
int MyFct()
    { return 42; }

int main()
    {
    std::chrono::milliseconds ms(3000);
    std::future<int> retval = std::async(MyFct);   // asynch function call
    std::this_thread::sleep_for(ms);
    std::cout << "\nThe value returned is "<< retval.get() << std::endl;
    }

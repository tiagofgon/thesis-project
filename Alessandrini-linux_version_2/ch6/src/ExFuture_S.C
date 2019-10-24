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

using namespace std; 

// This is the function that will be executed asynchronously
// ---------------------------------------------------------
auto MyFct()
    { return 42; }

int main()
    {
    chrono::milliseconds ms(3000);
    future<int> retval = async(MyFct);   // asynch function call
    //this_thread::sleep_for(ms);
    cout << "\nThe value returned is "<< retval.get() << std::endl;
    }

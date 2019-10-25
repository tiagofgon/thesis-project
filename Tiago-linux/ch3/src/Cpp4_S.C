// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// Cpp4_S.C : fourth C++11 example
// --------
// This example intends to show the side effects that occur
// when values are passed by reference to the thread function
// ----------------------------------------------------------

#include <iostream>
#include <thread>

// ---------------------------------------------------------
// This is a trivial thread function where a value is passed
// by reference. The purpose is to increment an integer value 
// external to the worker thread
// ---------------------------------------------------------
void ThreadFunc(int& N)
   { N += 10; }

int main(int argc, char* argv[])
   {
   int N = 10;
   std::cout << "\n Initial value of N = " << N << std::endl;

   // As discussed in the book, passing directly N has no effect on
   // this value. The following code may not work with some compilers
   // like Visual Studio 2013. This is why it is commented away
   // -------------------------------------------------------------
   //std::thread T1(ThreadFunc, N);  // launch thread to update N
   //T1.join();
   //std::cout << "First value of N = " << N << std::endl;

   // Now, the correct way, using the std::ref() construct
   // ----------------------------------------------------
   std::thread T2(ThreadFunc, std::ref(N));  // launch thread to update N
   T2.join();
   std::cout << "Second value of N = " << N << std::endl;

   return 0;
   }



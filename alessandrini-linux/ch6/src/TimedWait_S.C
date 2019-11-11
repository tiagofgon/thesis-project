// TimedWait_S.C
//
// This example shows the way of coding in C++11, a timed wait
// on a condition variable.
//
// Two things must be checked when the wait returns:
// -) The fact that the wait has not been timed out
// -) The fact that the wait is not spurious
//
// The protocol adopted by C++11 merges these two conditions
// The wait function returns !=0 only if the wait is not spureous
// and the waited event has actually happened.
//
// --------------------------------------------------------------

#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <condition_variable>

using namespace std;

condition_variable CV;
mutex my_mutex;
bool predicate = false;

// ---------------------------------------------------------------
// The C++11 documentation guarantees that this function is always
// called with the mutex that protects "predicate", locked. This
// is why there is no critical section here. 
// ---------------------------------------------------------------
bool MyPred()
   {
   bool retval = predicate;
   return retval;
   }

void worker_thread()
   {   
   chrono::duration<int, milli> delay(5000);
   this_thread::sleep_for(delay);
      {
      unique_lock<mutex> lock(my_mutex);
      predicate = true;
      }
   CV.notify_one();
   cout << "\n I/O operation terminated " << endl;
   }   

int main(int argc, char **argv)
   {
   bool retval;
   chrono::duration<int, milli> delay(1000);
   thread T(worker_thread);
   T.detach();
   do
      {
      unique_lock<std::mutex> my_lock(my_mutex);
      retval = CV.wait_for<int, milli>(my_lock, delay, MyPred);
      if(!retval) cout << "\n Timed out after 1 second" << endl; 
      }while(!retval);
   cout << "\n Wait terminated " << endl;
   }


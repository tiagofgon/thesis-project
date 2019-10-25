// AUTHOR: Victor Alessandrini, 2015
// VATH libray,in book "Shared Memory Application
// Programming"
// ***********************************************
// SafeCout.h
// Thread safe, mutex protected, output to
// stdout. Uses a std::ostringstream object to
// construct an output string; then this string is
// flushed to stdout in a mutex protected operation.
//
// The idea is to avoid locking a mutex while the 
// output message is being constucted
// -----------------------------------------------

#ifndef SAFECOUT_H
#define SAFECOUT_H

#include <mutex>
#include <iostream>
#include <string>
#include <sstream>

class SafeCout
   {
   private:
   std::mutex smutex; 

   public:
   SafeCout(){}

   ~SafeCout() {}

   void Flush(std::ostringstream& os) 
      {
      std::lock_guard<std::mutex> lock(smutex);
      std::string S = os.str();
      std::cout << S << std::endl; 
      os.str("");                   // reset
      }
   }; 

#endif

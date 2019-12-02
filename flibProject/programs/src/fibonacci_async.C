/* ---------------------------------------------------------------------------
                           Made by Tiago Gonçalves - 2019
 --------------------------------------------------------------------------*/
// ******************************************
// File fibonacci.C
// -------------
// Função fibonacci construida com a biblioteca future, que usa async
// 
// ------------------------------------------------------

#include <stdlib.h>
#include <iostream>
#include <TaskCentricPool.hpp>
#include <Timer.hpp>
#include <RandInt.hpp>
#include <SafeCout.hpp>
#include <SafeCounter.hpp>
#include <CpuTimer.hpp>
#include <atomic>
#include <memory>
#include <future>

int fibonacci(int n){
      if(n<2)
         return n;
      else
      {
         std::future<int> retval1 = async(fibonacci, n-1);
         std::future<int> retval2 = async(fibonacci, n-2);

         return retval1.get() + retval2.get();
      }
}
      
int main(int argc, char *argv[]) {
   if(argc != 2) {
      std::cout << "Please put one argumet as integer to calculate fibonacci" << std::endl;
      return 1;
   }
   int numero(atoi(argv[1]));

   std::future<int> retval = async(fibonacci, numero);

   std::cout << retval.get() << std::endl;
  
   return 0;
}  

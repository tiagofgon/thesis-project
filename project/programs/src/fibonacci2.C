// AUTHOR: Tiago Gonçalves, 2019
//
// ******************************************
// File fibonacci.C
// -------------
// Função fibonacci construida com a biblioteca future, que usa async
// 
// ------------------------------------------------------

#include <stdlib.h>
#include <iostream>
#include <NPool.h>
#include <Timer.h>
#include <RandInt.h>
#include <SafeCout.h>
#include <SafeCounter.h>
#include <CpuTimer.h>
#include <atomic>
#include <memory>
#include <future>


using namespace std;

int fibonacci2(int n){
      if(n<2)
         return n;
      else
      {
         future<int> retval1 = async(fibonacci2, n-1);
         future<int> retval2 = async(fibonacci2, n-2);

         return retval1.get() + retval2.get();
      }
}
      
int main(int argc, char *argv[]) {
  
   int numero(atoi(argv[1]));

   future<int> retval = async(fibonacci2, numero);

   cout << retval.get() << endl;
  
   return 0;
}  

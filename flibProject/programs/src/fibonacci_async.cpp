// AUTHOR: Tiago Gonçalves, 2019
//
// ******************************************
// File fibonacci.C
// -------------
// Função fibonacci construida com a biblioteca future, que usa async
// 
// ------------------------------------------------------

#include <iostream>
#include <future>

int fibonacci2(int n){
      if(n<2)
         return n;
      else
      {
         std::future<int> retval1 = std::async(fibonacci2, n-1);
         std::future<int> retval2 = std::async(fibonacci2, n-2);

         return retval1.get() + retval2.get();
      }
}
      
int main(int argc, char *argv[]) {
  
   int numero(atoi(argv[1]));
   std::future<int> retval = std::async(fibonacci2, numero);
   std::cout << retval.get() << std::endl;
  
   return 0;
}  

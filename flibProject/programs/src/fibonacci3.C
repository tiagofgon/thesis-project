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
#include <atomic>
#include <memory>
#include <future>


void fibonacci(std::promise<int>&& prom, int n){
    if(n < 2)
        prom.set_value(n);
    else
    {
        // define the promises
        std::promise<int> prom1;
        std::promise<int> prom2;

        // get the futures
        std::future<int> fut1 = prom1.get_future();
        std::future<int> fut2 = prom2.get_future();

        // calculate the result in a separat thread
        std::thread th1(fibonacci, move(prom1), n-1);
        std::thread th2(fibonacci, move(prom2), n-2);

        // get the result, vai buscar o valor de retorno que está presente nos futures
        int res1 = fut1.get();
        int res2 = fut2.get();

        th1.join();
        th2.join();

        prom.set_value(res1 + res2);
    }
}


      
int main(int argc, char *argv[]) {

    // define the promises
    std::promise<int> prom;                      // create promise

    // get the future
    std::future<int> fib_result = prom.get_future();    // engagement with future

    // calculate the result in a separat thread
    std::thread th1(fibonacci, std::move(prom), 15);
   
    // get the result, vai buscar o valor de retorno que está presente nos future
    int res = fib_result.get();
    th1.join();

    std::cout << res << std::endl;

    return 0;
}  

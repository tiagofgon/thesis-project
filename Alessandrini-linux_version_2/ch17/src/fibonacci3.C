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


using namespace std;



int foo(double a, char b, bool c){
   cout << "olaaaaa" << endl;
   return 29;
}

void print_int(std::future<int>& fut) {
    int x = fut.get(); // future would wait prom.set_value forever
    std::cout << "value: " << x << '\n';
}

using namespace std;

// int fibonacci(std::future<int> && fut){
//    int n = fut.get(); // future would wait prom.set_value forever
//    if(n < 2)
//       return n;
//    else
//    {
//       std::promise<int> prom1;                      // create promise
//       std::future<int> fib_result1 = prom1.get_future();    // engagement with future

//       std::promise<int> prom2;                      // create promise
//       std::future<int> fib_result2 = prom2.get_future();    // engagement with future

//       std::thread th1(fibonacci, ref(fib_result1));  // send future to new thread
//       std::thread th2(fibonacci, ref(fib_result2));  // send future to new thread

//       prom1.set_value(n);  // fulfill promise
//       prom2.set_value(n);  // fulfill promise
                                                 
//       th1.join();
//       th2.join();

//       // get the result, vai buscar o valor de retorno que está presente nos futures
//       int res1 = fib_result1.get();
//       int res2 = fib_result2.get();

//       return res1 + res2;
//    }
// }


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

    cout << res << endl;

    return 0;
}  

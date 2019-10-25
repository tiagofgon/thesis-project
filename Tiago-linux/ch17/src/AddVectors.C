// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ***********************************************
// File AddVectors.C
// ----------------------------------------------------------
// This is a very simple, embarassingly parallel code. We keep 
// adding two huge vectors using two worker threads. Each thread 
// operates on half of the vector range.
//
// In order to have measurable execution times, the addition
// operation is performed nsamples times (nsamples = 20000).
// The value of nsamples can be modified from the command
// line.
//
// This example employs the SPool utility. The ThreadRange
// member function is used to determine the range of vector
// indices allocated to each thread.
// ----------------------------------------------------------
#include <stdlib.h>
#include <CpuTimer.h>
#include <Rand.h>
#include <SPool.h>
#include <iostream>
#include <string>
#include <Timer.h>
#include <RandInt.h>

#define VECSIZE  60

using namespace std;

RandInt  R(1000);     // generates random integers in [0, 1000]
char thread1[VECSIZE];
char thread2[VECSIZE];
char thread3[VECSIZE];
char thread4[VECSIZE];

double A[VECSIZE];
double B[VECSIZE];
double C[VECSIZE];
long nsamples;

SPool *TH;   


//Mostra onde cada thread esteve a trabalhar no intervalo de dados
void print_shedule() {
   cout << "Thread 1:";
   for(int n=0; n<VECSIZE; n++){
      cout << thread1[n];
   }
   cout << "|" << endl;
   cout << "Thread 2:";
   for(int n=0; n<VECSIZE; n++){
      cout << thread2[n];
   }
   cout << "|" << endl;
   cout << "Thread 3:";
   for(int n=0; n<VECSIZE; n++){
      cout << thread3[n];
   }
   cout << "|" << endl;
   cout << "Thread 4:";
   for(int n=0; n<VECSIZE; n++){
      cout << thread4[n];
   }
   cout << "|" << endl;
}


// Função que simula trabalho de I/O, e guarda no array da thread respetiva
// em que posição está a trabalhar.
inline void doWork(int beg, int end){
   long timewait;
   Timer T;

   for(int n=beg; n<end; n++){
      int ir = R.draw();          // random integer in [0, 1000]
      timewait = (long) (ir);
      T.Wait(timewait);

      C[n] = A[n] + B[n]; 
      switch (TH->GetRank())
      {
         case 1:
         thread1[n] = '*';
         break;

         case 2:
         thread2[n] = '*';
         break;

         case 3:
         thread3[n] = '*';
         break;

         case 4:
         thread4[n] = '*';
         break;

         default:
         ;
      }
   }
}



void thread_fct(void *P)
   {
   int beg, end;
   //beg = 0;                    // initialize [beg, end) to global range
   //end = VECSIZE;
   //TH->ThreadRange(beg, end);  // now [beg, end) is range for this thread
   beg=0;end=0;


   // {
   //    std::pair<int, int> par = TH->shedule_static(VECSIZE);
   //    beg=par.first;
   //    end=par.second;
   //    //std::cout << "\n Thread: " << TH->GetRank() << " " << "computing in range [" << beg << " , " << end << ")" << std::endl;
      
   //    doWork(beg, end);
   // }


   // {
   //    while(end<VECSIZE) {
   //       std::pair<int, int> par = TH->shedule_dynamic(0, VECSIZE, 7);
   //       beg=par.first;
   //       end=par.second;
   //       //std::cout << "\n Thread: " << TH->GetRank() << " " << "computing in range [" << beg << " , " << end << ")" << std::endl;
         
   //       doWork(beg, end);

   //    }
   // }



   {
      while(end<VECSIZE) {
         std::pair<int, int> par = TH->shedule_guided(0, VECSIZE, 7);
         beg=par.first;
         end=par.second;
         //std::cout << "\n Thread: " << TH->GetRank() << " " << "computing in range [" << beg << " , " << end << ")" << std::endl;
         
         doWork(beg, end);
      }
   }


}


int main(int argc, char **argv)
   {
   CpuTimer TR;         // object to measure execution times
   Rand R(999);         // random generator used to initialize vectors

   if(argc==2) nsamples = atoi(argv[1]);
   else nsamples = 20000;
   TH = new SPool(4);
   
   // Vector components are initialized to random values in [0, 1]
   // ------------------------------------------------------------
   for(int n=0; n<VECSIZE; n++)
      {  
      A[n] = R.draw();
      B[n] = R.draw();
      thread1[n] = ' ';
      thread2[n] = ' ';
      thread3[n] = ' ';
      thread4[n] = ' ';
      }

   
   TR.Start();
   TH->Dispatch(thread_fct, NULL);
   TH->WaitForIdle();
   TR.Stop();

   TR.Report();


   print_shedule();


   delete TH;
   }

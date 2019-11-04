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
#define VECSIZE  200000

using namespace std;

double A[VECSIZE];
double B[VECSIZE];
double C[VECSIZE];
long nsamples;

SPool *TH;   

void thread_fct(void *P)
   {
   int beg, end;
   Rand R(999);
   beg = 0;                    // initialize [beg, end) to global range
   end = VECSIZE;
   TH->ThreadRange(beg, end);  // now [beg, end) is range for this thread
   std::cout << "\n Thread computing in range [" << beg << " , " << end
             << ")" << std::endl;
   
   for(int j=0; j<nsamples; j++)
      {
      for(int n=beg; n<end; n++) C[n] = A[n] + B[n];
      }
   }


int main(int argc, char **argv)
   {
   CpuTimer TR;         // object to measure execution times
   Rand R(999);         // random generator used to initialize vectors

   if(argc==2) nsamples = atoi(argv[1]);
   else nsamples = 20000;
   TH = new SPool(2);
   
   // Vector components are initialized to random values in [0, 1]
   // ------------------------------------------------------------
   for(int n=0; n<VECSIZE; n++)
      {
      A[n] = R.draw();
      B[n] = R.draw();
      }
   
   TR.Start();
   TH->Dispatch(thread_fct, NULL);
   TH->WaitForIdle();
   TR.Stop();

   // Check the vector addition for indices 0 and 4
   // ---------------------------------------------
   cout << "A[0] = " << A[0] << "  B[0] = " << B[0] << "  C[0] = " << C[0]
        << "\n";
   cout << "A[4] = " << A[4] << "  B[4] = " << B[4] << "  C[4] = " << C[4]
        << endl;
   TR.Report();
   delete TH;
   }

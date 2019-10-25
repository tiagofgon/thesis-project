// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// File AtomicTestOmp.C
//
// MonteCarlo computation of PI - OpenMP version
//
// In this example, acceptances are collected by performing an 
// atomic update of a long. There is excessive mutual exclusion, 
// but performance is reasonable
// -------------------------------------------------------------
#include <stdlib.h>
#include <CpuTimer.h>
#include <iostream>
#include <Rand.h>
#include <omp.h>               // new

using namespace std;
long nsamples;
long C;       // counts accepted events

void task_fct()
   {
   double x, y;
   int rank = omp_get_thread_num();
   Rand R(999*(rank+1));
   std::cout << "\n Thread " << rank << " active" << std::endl;

   for(int n=0; n<nsamples; n++)
      {
      x = R.draw();
      y = R.draw();
      if((x*x+y*y) <= 1.0 )
         {
         #pragma omp atomic update
         C++;
         }
      }
   }
   

int main(int argc, char **argv)
   {
   CpuTimer TR;
   double x, y, pi;
   int nTh;
   long cnt;

   if(argc==1)
      {
      nTh = 2;
      nsamples = 100000000;
      }
   if(argc==2)
      {
      nsamples = 100000000;
      nTh = atoi(argv[1]);
      }
   if(argc==3)
      {
      nTh = atoi(argv[1]);
      nsamples = atoi(argv[2]);
      }
   nsamples /= nTh;

   TR.Start();
   omp_set_num_threads(nTh);
   #pragma omp parallel
      {
      task_fct();
      }

   pi = 4.0 * (double)C / (nTh*nsamples);
   // ---------------------------------------------------
   TR.Stop();
   cout << "\n Value of PI = " << pi << endl;
   TR.Report();
   }

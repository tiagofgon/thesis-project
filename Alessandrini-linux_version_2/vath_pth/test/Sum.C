// **************************************
// Copyright (c) 2011 Victor Alessandrini
// All rights reserved.
// **************************************
// ***********************************************************
// This file is Sum.C
//
// This code is an "embarassingly parallel" calculation of the
// area defined by a function FCT(x), the real x axis and two
// limit values x=a and x=b.
//
// Uses the SPool thread pool facility.
//
// The area is divided into several subareas, and each subarea 
// is computed by a different thread.
//
//                         *
//                       *   *
//                    *        *  FCT(x)
//                *                *
//        *                             *
//
//
// -------a-----------------------------b---------> x    
//
// The main function collects the results provided by each
// thread, and provides the final answer. 
// ***********************************************************

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SPool.h>
#include <Timer.h>
#include <Reduction.h>

#define ErrExit(A) fprintf(stderr, A); exit(0)

// ---------------------------------------
// This is the definition of the function 
// to be integrated. This is the part of
// the code that must be changed.
// --------------------------------------

double FCT(double a)
   {
   return sin(a);
   }

// -----------------------
// Data for multithreading
// -----------------------

SPool *TP;             // declare global thread pool
Reduction<double> RD;  // to recover result
double A, B;
int nTh;

// ------------------------------------
// This is the generic thread function
// ------------------------------------

void thread_fct (void *arg)
   {
   double a, b, size; 
   int n, i, j, rank;                   
   double s, snew, x, tnm, sum, del; 

   // ---------------------------------------
   // Now, we vill get the integration limits 
   // for this thread
   // ----------------------------------------
   
   size = (B-A)/nTh;
   rank = TP->GetRank();
   a = A+(rank-1)*size;
   b = a+size;

   // --------------------------------------------
   // Now we compute the sum from a to b of FCT(x) 
   // using an improved method that stops after a
   // precision of 0.001 is reached (taken from
   // Numerical Recipes in C). 
   // From now on, a and b are NOT the limits of the 
   // full domain, they are the limits of the subdomain
   // that is handled by the current thread 
   // -------------------------------------------*/
   
   n = 1;
   i = 1; 
   snew = 0.5*(b-a)*(FCT(a)+FCT(b));
   do
     {
     s = snew;
     i <<=1;
     tnm = i;
     del = (b-a)/tnm;
     x = a+0.5*del;
     for(sum=0.0, j=1; j<=i; j++, x+=del) sum += FCT(x);
     snew = 0.5*(s+(b-a)*sum/tnm);
     }while(fabs(s-snew)>0.0001);
   printf("\n Thread %d computed area of %g \n", rank, snew);
    
   RD.Accumulate(snew);         // Accumulate return value 
   }


int main (int argc, char *argv[])
   {
   int n, status;
   Timer T;

   A =1; B=2; nTh=2;
   printf("\n Area computation using SPool class \n");
   
   // --------------------------------------------
   // Create thread pool, initialize arguments, 
   // dispatch and wait for idle.
   // --------------------------------------------
   
   TP = new SPool(nTh);
   TP->Dispatch(thread_fct, NULL);
   TP->WaitForIdle();
   printf("\n result = %g\n\n", RD.Data());

   delete TP;
   return 0;
   }


// **************************************
// Copyright (c) 2011 Victor Alessandrini
// All rights reserved.
// **************************************
// *****************************************************************
// Qsort.C
// ----------
// Uses the partition algorithm (mutating class) to implement
// the quicksort algorithm
//
// The algorithm works as follows:
// ------------------------------
//
// -) We have an integer array in [beg, end) (ussual conventions).
//
// -) Decrement end (--end) so that now end points to last element.
//
// -) This last element will be chosen as pivot, and we will move
//    to the front of the container all elements less that "pivot"
//    using the partition algorithm.
//
// -) The partition algorithm returns a pointer "middle" to the first 
//    element NOT smaller than "pivot". Remember that "pivot" is still 
//    sitting at the end of the container.
//
// -) Swap the values of "*middle" and "*end". Now, *middle=pivot
// -) Call recursively the qsort routine to the arrays:
//    [beg, middle) and [middle++, end++). Notice that the "pivot"
//    element is already in place
//
// ***************************************************************** 

#include <iostream> 
#include <vector> 
#include <algorithm> 

using namespace std;


// ********************************
// Auxiliary function object class
// ********************************

class LessThan
   {
   private:
   long value;

   public:
   LessThan(long i) : value(i) {}
   bool operator()(long i)
      {
      if(i<value) return true;
      else return false;
      }
   };

// *****************************
// The basic quicksort algorithm
// *****************************

void qsort(long *begin, long *end)
   {
   if(begin != end)
      {
      --end;
      long pivot = *end;
      long *middle = partition(begin, end, LessThan(pivot));
      swap(*end, *middle);
      qsort(begin, middle);
      qsort(++middle, ++end);
      }
   }

// *********************************
// The recursive quicksort algorithm
// *********************************

void QsortR(long *begin, long *end, long G)
   {
   if(begin != end)
      {
      cout << "\n Recursive call for range " << *begin 
           << "   " << *end << endl;
      --end;
      long pivot = *end;
      long *middle = partition(begin, end, LessThan(pivot));
      swap(*end, *middle);
      if( (middle-begin)>G) QsortR(begin, middle, G);
      else qsort(begin, middle);
      if( (end-middle)>G) QsortR(++middle, ++end, G);
      else qsort(++middle, ++end);
      }
   }



// *************
// A simple test
// *************

int qtest(long n, long G)
   {
   long *a = new long[n];
   
   for(long i=0; i<n; i++) a[i]=i;
   random_shuffle(a, a+n);
   
   cout << "testing sequential qsort" << endl;
   cout << "Sorting " << n << " integers" << endl;
   QsortR(a, a+n, G);

   // Check sorting
   // -------------
   for(long i=0; i<(n-1); ++i)
      {
      if(a[i] >= a[i+1] || a[i] != i)
         {
         cout << "Sort failed at location i=" << i << endl;
         delete[] a;
         return 1;
         }
      }
   cout << "Sort succeeded. " << endl;
   return 0;
   }


// ***********
// MAIN CODE
// ***********

int main(int argc, char* argv[])
   {
   long n, G;
   cout << "\n *** Testing quicksort\n" << endl;
   n = 10000000;
   G = 1000;
   return qtest(n, G);
   }

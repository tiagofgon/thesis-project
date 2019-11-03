// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// qsort.C
// ----------
// Uses the partition algorithm (mutating class) to implement
// the quicksort algorithm. THIS IS A SEQUENTIAL CODE
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
//
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
   int value;

   public:
   LessThan(int i) : value(i) {}
   bool operator()(int i)
      {
      if(i<value) return true;
      else return false;
      }
   };

// ************************
// The quicksort algorithm
// ************************

void qsort(int *begin, int *end)
   {
   if(begin != end)
      {
      --end;
      int pivot = *end;
      int *middle = partition(begin, end, LessThan(pivot));
      swap(*end, *middle);
      qsort(begin, middle);
      qsort(++middle, ++end);
      }
   }

// **********************************
// Auxiliary function, to compute the
// size of an integer container
// **********************************
int ArraySize(int *beg, int *end)
   {
   int sz = end-beg;
   return sz;
   }

// *************
// A simple test
// *************

int qtest(int n)
   {
   int *a = new int[n];
   
   for(int i=0; i<n; i++) a[i]=i;
   random_shuffle(a, a+n);
   
   cout << "Sorting " << n << " integers" << endl;
   cout << "Container size is " << ArraySize(a, a+n) << endl;
   qsort(a, a+n);

   // Check sorting
   // -------------
   for(int i=0; i<(n-1); ++i)
      {
      if(a[i] >= a[i+1] || a[i] != i)
         {
         cout << "Sort failed at location i=" << i << endl;
         delete[] a;
         return 1;
         }
      }
   cout << "Sort succeeded. " << endl;
   delete[] a;
   return 0;
   }


// ***********
// MAIN CODE
// ***********

int main(int argc, char* argv[])
   {
   int n = 100000;
   if(argc>1) n=atoi(argv[1]);
   return qtest(n);
   }

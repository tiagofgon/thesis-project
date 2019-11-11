// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// QsortOmp.C
// 
// This example shows the utility of the "task" construct in handling
// recursiven irregular problems. It uses the STL partition algorithm 
// (mutating class) to implement the quicksort algorithm
//
// The algorithm works as follows:
// ------------------------------
//
// -) We have an integer array in [beg, end) (ussual conventions).
//    where beg end end are integer pointers. Remember that end is a
//    "past the end" pointer, and the integer *end does not exist.
//
// -) Decrement end (--end) so that now end points to the last element
//    in the array.
//
// -) This last element is chosen as pivot, and we will move to the
//    front of the container all elements less than "pivot" using the 
//    partition STL algorithm.
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
#include <omp.h> 

using namespace std;

int    G;         // granularity

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


// **********************************
// The sequential quicksort algorithm
// **********************************

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



// ================================
// The parallel recursive algorithm.
// ================================

void Pqsort(int *a, int *b)
   {
   cout << "\n Inside Pqsort" << endl;
   --b;
   int pivot = *b;
   int *middle = partition(a, b, LessThan(pivot));
   swap(*b, *middle);

   if( (middle-a) < G) qsort(a, middle);
   else 
      {
      #pragma omp task untied
         {
         cout << "Child task 1 submitted " << endl;
         Pqsort(a, middle);
         }
      }
   
   ++middle;
   ++b;
   if( (b-middle) < G) qsort(middle, b);
   else 
      {
      #pragma omp task untied
         {
         cout << "Child task 2 submitted " << endl;
         Pqsort(middle, b);
         }
      }
   }


// ------------------
// Auxiliary function
// ------------------
void CheckSort(int *A, int N)
   {
   for(int i=0; i<(N-1); ++i)
      {
      if(A[i] >= A[i+1] || A[i] != i)
         cout << "Sort failed at location i=" << i << endl;
      }
   cout << "Sort succeeded. " << endl;
   }


// ***********
// MAIN CODE
// ***********

int main(int argc, char* argv[])
   {
   int n = 100000;
   G = 20000;
   if(argc>1) n=atoi(argv[1]);

   int *a = new int[n];
   for(int i=0; i<n; i++) a[i]=i;
   random_shuffle(a, a+n);

   #pragma omp parallel num_threads(2)
      {
      #pragma omp single
         Pqsort(a, a+n);
      }

   CheckSort(a, n);
   return 0;
   }

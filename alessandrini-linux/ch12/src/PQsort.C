// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// PQsort.C
// ----------
// Uses the partition algorithm (mutating class) to implement
// the quicksort algorithm
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
#include <NPool.h> 

using namespace std;

NPool *NP;       // Reference to thread pool
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

// **********************************
// The parallel recursive algorithm.
// **********************************
class QsortTask: public Task
   {
   private:
    int *a, *b;

   public:
    QsortTask(int *_a, int *_b): Task(), a(_a), b(_b) {}
    void ExecuteTask()
       {
       --b;
       int pivot = *b;
       int *middle = partition(a, b, LessThan(pivot));
       swap(*b, *middle);

       if( (a-middle) < G) qsort(a, middle);
       else 
          {
          QsortTask *T = new QsortTask(a, middle);
          NP->SpawnTask(T, false);
          cout << "Child task spawned " << endl;
          }
   
       ++middle;
       ++b;
       if( (middle-b) < G) qsort(middle, b);
       else 
          {
          QsortTask *T = new QsortTask(middle, b);
          NP->SpawnTask(T, false);
          cout << "Child task spawned " << endl;
          }
      }
   };


// *************
// A simple test
// *************
int Qtest(int n)
   {
   int *a = new int[n];
   
   for(int i=0; i<n; i++) a[i]=i;
   random_shuffle(a, a+n);
   
   cout << "Sorting " << n << " integers" << endl;
   QsortTask *T = new QsortTask(a, a+n);
   int ID = NP->SubmitJob(T);
   NP->WaitForJob(ID);

   // Check sorting
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
   return 0;
   }


// ***********
// MAIN CODE
// ***********

int main(int argc, char* argv[])
   {
   int n = 100000;
   G = 20000;
   NP = new NPool(2, 20);
   if(argc>1) n=atoi(argv[1]);
   Qtest(n);
   return 0;
   }

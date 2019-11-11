// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// for_each.C
//
// This example is sequential code. It shows the
// usage of the STL for_each algorithm to perform
// a map operation on all the elements of a container.
//
// Useful to compare performances with the parallel
// version using the TBB parallel_for_each algorithm.
// -------------------------------------------------

#include <iostream> 
#include <stdlib.h> 
#include <vector> 
#include <list> 
#include <algorithm> 
#include <print_container.h>
#include <math.h>
#include <Rand.h>

using namespace std; 
 
// ---------------------------------------------------
// Use a function object to display doubles in a more
// sophisticated way. Internal state counts the number
// of doubles displayed. Every five doubles, a newline
// is sent.
// ---------------------------------------------------
class ShowDouble
   {
   private:
   int count;
 
   public:
   ShowDouble() : count(0) {}
   void operator()(const double& d)
      {
      cout << d << "  ";
      count++;
      if( !(count%5) ) cout << endl;
      }
   };

// --------------------------------------------------
// Define a function object that modifies a double value
// by searching for a close random number within a given
// precision.
// ------------------------------------------------

class Replace 
   { 
   private:
     double precision;

   public:
    Replace(double d) : precision(d) {}

    void operator()(double& d)
       {
       Rand R(999);
       double x;
       do
          {
          x = R.draw();
          }while( fabs(x-d)>precision );
       d = x;
       }
   };


// *****************************************************
 
int main(int argc, char **argv) 
   {
   double precision;

   // read precision from command line
   // --------------------------------
   if(argc==2) precision = atof(argv[1]);
   else precision = 0.05;
   cout << "\n Precision is " << precision << endl;   

   // Declare vector of size 30.
   // --------------------------
   vector<double> V(30);

   // Declare random number generator in [0, 1]
   // ----------------------------------------- 
   Rand rd(777);
   for(int n=0; n < V.size(); ++n) V[n] = rd.draw(); 
 
   cout << "\nContents of V: " << endl; 
   for_each(V.begin(), V.end(), ShowDouble()); 
   cout << "\n"; 

   // --------------------------------------------------
   // Next, we modfy the content of the vector V
   // --------------------------------------------------
   Replace F(precision); 
   for_each(V.begin(), V.end(), F);
 
   cout << "\nContents of V, after replacement: " << endl; 
   for_each(V.begin(), V.end(), ShowDouble()); 
   cout << "\n"; 

   // -------------------------------------------------------
   // Create a list<double> with the content of the vector V
   // using the "range" [beg, end) constructor. Notice that
   // we are passing a range in a container of a different kind.
   // This works.
   // ------------------------------------------------------
   list<double> L(V.begin(), V.end());
   print_container(L, "Content of list", 6);

   // -----------------------------------
   // Replace now the elements of the list
   // -----------------------------------
   Replace G(0.05);
   for_each(L.begin(), L.end(), G);
   print_container(L, "Content of list after replacement", 6);
   return 0; 
   }



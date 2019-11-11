// **************************************
// Copyright (c) 2011 Victor Alessandrini
// All rights reserved.
// **************************************
/* ==============================================================
 * ThSetest.C
 * Tests the ThSet class that runs a set of threads
 *
 * NOTICE:
 * ------
 * If the set does not include the master thread, then Nthreads=N
 * and the GetRank() function returns integer in [1,N]
 *
 * If the set DOES include the master thread, then Nthreads=N+1
 * and the GetRank() function returns integer in [0,N], 0 being
 * the rank of the master thread.
 *
 * If the master thread is included, the submssion request must
 * take this into account (provide work for N+1 threads). Eventual
 * barriers included in the code to synchro,ize the (N+1) working
 * threads must also take this into account.
 *
 * The Run() function launches the worker threads, and also executes
 * the job attributed to the master thread. When master is included,
 * Run() returns after the master thread terminates its share of
 * work.  
 * =============================================================*/

#include <iostream>
#include <ThSet.h>
#include <stdlib.h>
#include <string>
#include <Reduction.h>

ThSet *Th;
double *A, *B;
Reduction<double> R;

// ----------
// First test
// ----------

void *th_fct(void *arg)
    {
    int rank = Th->GetRank();
    std::cout << "\n From thread " << rank << " : Hello, world!" << std::endl;
    return NULL;
    }

// -----------
// Second test
// -----------

void *th1(void *arg)
    {
    int rank = Th->GetRank();
    std::string *s = (std::string *) arg;
    std::cout << "\n From thread " << rank << "  " << *s << std::endl;
    return NULL;
    }

void *th2(void *arg)
    {
    int rank = Th->GetRank();
    std::string *s = (std::string *)arg;
    std::cout << "\n From thread " << rank << "  " << *s << std::endl;
    return NULL;
    }

void *th3(void *arg)
    {
    int rank = Th->GetRank();
    std::string *s = (std::string *)arg;
    std::cout << "\n From thread " << rank << "  " << *s << std::endl;
    return NULL;
    }

// ------------------------------------------
// Third test: scalar product of two vectors
// Testing inclusion or exclusion of master,
// and the ThreadRange function
// ------------------------------------------

void *dot_prod(void *arg)
   {
   int n, rank, beg, end;
   double result;

   // Determine rank and the working index range 
   // for this thread
   // ------------------------------------------   
   rank = Th->GetRank();
   beg = 0;
   end = 1000;
   Th->ThreadRange(beg, end);
   std::cout << "\nRange for thread " << rank << " is: " << beg << " "
             << end << std::endl;

   // Compute partial dot product value
   // ---------------------------------
   result = 0.0;
   for(n=beg; n<end; n++) result += (A[n]*B[n]);

   // Perform reduction
   // -----------------
   R.Accumulate(result);
   return NULL;
   }

// ------------------------------------------
// Main function.
// Get the number of threads from the command
// line
// ------------------------------------------

int main(int argc, char **argv)
    {
    int status, nTh;
    char ch;

    if(argc!=2)
       {
       std::cout << "Usage : ./tset nThreads " << std::endl;
       exit(0);
       }
    nTh = atoi(argv[1]);
   
    // ---------------------------------------------
    // Test 1 : nTh working threads, master not 
    // included
    // --------------------------------------------- 
    Th = new ThSet(nTh);
    Th->Run(th_fct, NULL);    // launch threads
    Th->Join();
    std::cout << "\n From main : thread joined " << std::endl;
    delete Th;

    std::cout << "\n Enter a char to perform test 2 " << std::endl;
    std::cin >> ch;

    // ---------------------------------------------
    // Test 2 : nTh working threads, master included 
    // --------------------------------------------- 
    Th = new ThSet(nTh, true);
    Th->Run(th_fct, NULL);    // launch threads
    Th->Join();
    std::cout << "\n From main : thread joined " << std::endl;
    delete Th;

    std::cout << "\n Enter a char to perform test 3 " << std::endl;
    std::cin >> ch;

    // --------------------------------------
    // Prepare data structures for next tests
    // --------------------------------------
    std::string s1 = "This is string s1"; 
    std::string s2 = "This is string s2"; 
    std::string s3 = "This is string s3"; 
    ThreadFct thf1(th1, (void*)&s1);
    ThreadFct thf2(th2, (void*)&s2);
    ThreadFct thf3(th3, (void*)&s3);
    ThreadFctList TFL;
    TFL.Attach(thf1);
    TFL.Attach(thf2);
    TFL.Attach(thf3);
    
    // -------------------------------------------------
    // Test 3 : 3 working threads, master not included
    // submitting a list of thread functions 
    // -------------------------------------------------

    Th = new ThSet(3);
    Th->Run(TFL);    // launch threads
    Th->Join();
    std::cout << "\n From main : thread joined " << std::endl;
    delete Th;
    
    std::cout << "\n Enter a char to perform test 4 " << std::endl;
    std::cin >> ch;

    // -------------------------------------------------
    // Test 4 : 2 working threads plus master (3 THREADS)
    // submitting a list of thread functions 
    // -------------------------------------------------

    Th = new ThSet(2, true);
    Th->Run(TFL);    // launch threads
    Th->Join();
    std::cout << "\n From main : thread joined " << std::endl;
    delete Th;

    std::cout << "\n Enter a char to perform test 5 " << std::endl;
    std::cin >> ch;

    // --------------------------------------------------------
    // Test 5: scalar product of two vectors, main not included
    // See how ThreadRange works.
    // First, allocate and initialize the target vectors.
    // --------------------------------------------------------

    A = new double[1000];
    B = new double[1000];
    for(int n=0; n<1000; n++)
       {
       A[n] = 0.1;
       B[n] = 0.1;
       }

    Th = new ThSet(nTh);
    Th->Run(dot_prod, NULL);    // launch threads
    Th->Join();
    std::cout << "\n Dot product is : " << R.Data() << std::endl;
    delete Th;
    
    std::cout << "\n Enter a char to perform test 6 " << std::endl;
    std::cin >> ch;

    // --------------------------------------------------------
    // Test 6: scalar product of two vectors, main included
    // See how ThreadRange works.
    // Notice that THE SAME thread function, dot_prod, can
    // be used.
    // --------------------------------------------------------

    R.Reset();                  // reset the accumulator
    Th = new ThSet(nTh, true);
    Th->Run(dot_prod, NULL);    // launch threads
    Th->Join();
    std::cout << "\n Dot product is : " << R.Data() << std::endl;
    delete Th;
    
    return 0;
    }
	

/*********************************************************/

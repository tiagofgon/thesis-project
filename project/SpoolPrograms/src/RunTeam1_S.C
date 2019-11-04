// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
// RunTeam1_S.C
// 
// The purpose of this example is twofold. First, we
// show how to manage a team  of worker threads. A 
// team of worker threads is launched. The number of 
// threads is read from the command line (default is 4).
//
// Then, we show the way to use thread identities. The
// worker thread identities are stored in an array when
// the threads are created. This array in needed to join
// the threads later on, but it can also be used to
// identify each thread with an integer rank, given by
// its place in the array.
// -------------------------------------------------

#include <iostream>
#include <thread>
#include <chrono>

// Global data
// -----------
std::thread       **Wth;    // array of thread pointers
int               nTh;

// ------------------------------------------
// Auxiliary function to compute thread ranks
// This function:
// -) gets the identity of the caller thread
// -) scans the array where the worker thread
//    identities are stored
// -) In this way, it finds its place in the
//    array. Returns the array index
// ------------------------------------------
int GetRank()
   {
   std::thread::id my_id, target_id;
   int n, my_rank;

   my_id = std::this_thread::get_id();
   n = 0;
   do
      {
      n++;
      target_id = Wth[n]->get_id();
      } while(my_id != target_id && n < nTh); 

   if(n<=nTh) my_rank = n;  // if rank OK, return
   else my_rank = (-1);     // else, return error
   return my_rank;
   }

// Thread function
// ---------------
void helloFunc()
   {
   // ---------------------------------------------
   // create a time duration object and wait. The
   // duration interval is equal to the thread rank
   // in seconds.
   // ---------------------------------------------
   std::chrono::milliseconds delay1(500);
   std::this_thread::sleep_for(delay1);

   int rank = GetRank();
   std::chrono::seconds delay2(rank);
   std::this_thread::sleep_for(delay2);
   std::cout << "Hello from thread number " << rank 
             << std::endl;
   }


int main(int argc, char* argv[])
   {
   if(argc==2) nTh = atoi(argv[1]);
   else nTh = 4;

   // here, threads are created and launched
   // --------------------------------------
   Wth   = new std::thread*[nTh+1];

   for(int n=1; n<=nTh; ++n)
      Wth[n] = new std::thread(helloFunc);

   for(int n=1; n<=nTh; ++n) Wth[n]->join();
   std::cout << "main: done" << std::endl;

   // Deallocate memory
   // -----------------
   for(int n=1; n<=nTh; ++n) delete Wth[n];
   delete [] Wth;
   return 0;
   }



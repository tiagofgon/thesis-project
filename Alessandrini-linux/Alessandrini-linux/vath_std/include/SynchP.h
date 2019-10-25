// AUTHOR: Victor Alessandrini, 2015
// VATH libray,in book "Shared Memory Application
// Programming"
// =====================================================
// SynchP.h
// Template code.

// This is a synchronization point among several threads
// with one producer that writes a value of type T, and
// several consumers
// Different synchronization strategy than SynchPoint<T>
//
// In this scheme:
//
// *) state = false means that object is ready to be reused
//    because previos data has been read.
//
// *) Producer thread only waits if state=true (object not
//    ready). Once value is written, it does not wait. The
//    Put() function specifies how many threads must read
//    the data.
//
// *) Consumes will eventually wait for true, read the data
//    and decrease the counter. When counter=0, state is
//    changed to false and the change signaled.
//
// ========================================================

#ifndef SYNCH_P
#define SYNCH_P

#include <mutex>
#include <condition_variable>

template <class T>
class SynchP
   {
   private:

   T      element;
   bool   state;
   int    counter;
   std::mutex                slock;
   std::condition_variable   scond;

   public:

   // Constructor and destructor
   // --------------------------
   SynchP()
      {
      state = false;
      counter = 0;
      }

   ~SynchP() {}


   // ----------------------------------------------
   // This function posts a value. It only waits if
   // target object is not ready
   // ----------------------------------------------
   void Post(T& elem, int nReaders)
      {
      std::unique_lock<std::mutex> lock(slock);
      
      // wait if object not ready
      // ------------------------
      while(state==true) scond.wait(lock);
      
      // here, state is false
      // --------------------
      element = elem; 
      counter = nReaders;
      state = true;
      scond.notify_all();
      }    


   // ------------------------------------------
   // This function waits at the synchronization
   // point and gets a T value
   // ------------------------------------------
   T Get()
      {
      T retval = T();

      std::unique_lock<std::mutex> lock(slock);

      // wait for true, signaled after a write
      // -------------------------------------
      while(state==false) scond.wait(lock);
      
      // here, state is true, read
      // -------------------------
      if(state==true)
          {
          retval = element;
          counter--;
          }
      
      // If I am the last to read, change state to
      // false and signal the change
      // -----------------------------------------
      if(counter==0)
          {
          state = false;
          scond.notify_one();
          }
      return retval;
      }    

   // end of class
   };
          
// *************************************************

#endif  // SYNCH_P

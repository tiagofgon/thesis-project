// AUTHOR: Victor Alessandrini, 2015
// VATH libray,in book "Shared Memory Application
// Programming"
// *********************************************
// OSynchP.h
//
// OpenMP implementation of the of SynchP utility,
// used to synchronize the write of a data value by
// a producer, with the read of the data value by
// one or several consumers.
// **************************************

#ifndef OMP_SYNCH_P
#define OMP_SYNCH_P

#include <OmpBLock.h>

template <class T>
class OSynchP
   {
   private:

   T           element;
   int         counter;
   omp_lock_t  clock;
   OBLock      BL; 

   public:

   // Constructor and destructor
   // --------------------------
   OSynchP()
      { 
      counter = 0;
      omp_init_lock(&clock);
      }

   ~OSynchP() 
      { omp_destroy_lock(&clock); }

   // ---------------------------------------------
   // This function posts a value. It does not wait
   // ---------------------------------------------
   void Post(T& elem, int nReaders)
      {
      // wait if object not ready
      // ------------------------
      BL.Wait_Until_False();
      
      // here, BL state is false
      // -----------------------
      omp_set_lock(&clock);
      element = elem; 
      counter = nReaders;
      BL.SetValue(true);
      omp_unset_lock(&clock);
      }    

   // ------------------------------------------
   // This function waits at the synchronization
   // point and gets a T value
   // ------------------------------------------
       
   T Get()
      {
      T retval = T();

      // wait for true, signaled after a write
      // -------------------------------------
      BL.Wait_Until_True();
 
      // here, BL state is true. Read
      // ----------------------------
      omp_set_lock(&clock);
      retval = element; 
      counter--;
      if(counter==0) BL.SetValue(false);
      omp_unset_lock(&clock);

      return retval;
      }    

   // end of class
   };
          
// *************************************************

 
#endif  // OMP_SYNCH_P

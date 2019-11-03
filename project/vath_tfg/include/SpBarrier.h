// AUTHOR: Victor Alessandrini, 2015
// VATH libray,in book "Shared Memory Application
// Programming"
// ***********************************************
// SpBarrier.h
// Threads in barrier perform a spin wait. In addition,
// the custom SpinLock class is used, so that this 
// synchronization operates mainly in user space.
// --------------------------------------------

#ifndef  SP_BARRIER_H
#define  SP_BARRIER_H

#include <SpinLock.h>

class  SpBarrier
   {
   private:
     SpinLock    lmutex;        // Control access to barrier
     SpinLock    lwait;         
     int         nTh;           //  number of threads required
     int         counter;       // counts active threads      
     bool        cycle;         // toggles between successive calls
 
   public:
     SpBarrier(int count);
     ~SpBarrier();
     void Wait();
   };

#endif  /* SPBARRIER_H */

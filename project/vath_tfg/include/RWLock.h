// **************************************
// Copyright (c) 2016 Victor Alessandrini
// All rights reserved.
// **************************************
// =================================================
// RWlock.h
// Header file for read-write lock.
// ------------------------------------------------
// Algorithm taken from "The Art of Multiprocessor
// Programming" M. Herlihy and N. Shavit; chapter 8
// section 8.3.2
//
// We have in this code two predicates: "writer" and
// "read_acquires=read_releases", but only one mutex
// "rw_mutex" and CV "cond" to wait on both predicates.
// This is correct. Remember that the condition variable
// knows nothing about the associated predicate.
//
// Transitions between "read" and "write" regions.
// A "write" region is exclusive, and, when a thread is
// writing, any other request (read or write) wait for
// the "write=false" event, that indicates the termination
// of the current ongoing write.

// A thread that terminates writing sets "write=false".
// and, at this point, all waiting readers and writers
// are waken up. Readers proceed to read. When a writer takes
// ownership of the mutex, it sets sets "write=true", so that 
// any further read and write request is blocked, and waits 
// until the ongoing readers terminate. Then it proceeds to
// write
//
// It seems that in this algorithm, waiting readers take
// precedence on waiting writes.
// ======================================================

#ifndef RWLOCK_H
#define RWLOCK_H

#include <mutex>
#include <condition_variable>

class RWLock
    {
    private:
      int   activeReaders;
      bool  writer;
      std::mutex rw_mutex;
      std::condition_variable  cond;

      void lockRead()
         {
         std::unique_lock<std::mutex> lock(rw_mutex);
         while(writer) cond.wait(lock); 
         activeReaders++;
         }

      int trylockRead()
         {
         int retval;
         std::unique_lock<std::mutex> lock(rw_mutex);
         if(writer) retval = 0; 
         else
            {
            activeReaders++;
            retval = 1;
            }
         return retval;
         }

      void unlockRead()
         {
         std::unique_lock<std::mutex> lock(rw_mutex);
         activeReaders--;
         if(activeReaders==0) cond.notify_all();
         }


      void lockWrite()
         {
         std::unique_lock<std::mutex> lock(rw_mutex);
         while(writer) cond.wait(lock); 
         writer = true;
         while(activeReaders) cond.wait(lock);
         }

      int trylockWrite()
         {
         int retval;
         std::unique_lock<std::mutex> lock(rw_mutex);
         if(writer) retval = 0;
         else
            { 
            writer = true;
            retval = 1;
            while(activeReaders) cond.wait(lock);
            }
         return retval;
         }

      void unlockWrite()
         {
         writer = false;
         cond.notify_all();
         }


    public:
      RWLock()
         {
         activeReaders = 0;
         writer = false;
         }

      ~RWLock() {}

      int Lock(bool B)
          {
          if(B==true) lockWrite();
          else lockRead();
          return 1;
          }

      int  Trylock(bool B) 
          {
          int retval;
          if(B==true) retval = trylockWrite();
          else retval = trylockRead();
          return retval;
          }

      int  Unlock(bool B)
          {
          if(B==true) unlockWrite();
          else unlockRead();
          return 1;
          }
    };

#endif /* F_RWLOCK_H */

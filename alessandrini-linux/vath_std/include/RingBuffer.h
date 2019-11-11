// AUTHOR: Victor Alessandrini, 2015
// VATH libray,in book "Shared Memory Application
// Programming"
// ***********************************************
// 
// RingBuffer class, using standard mutex locking
// (there are no atomic operations in Pthreads).
//
// This is a memory buffer connecting one producer 
// and one consumer thread.
//
// Does not work for several producers and/or
// consumers.
// --------------------

#ifndef RING_BUFFER
#define RING_BUFFER
#define VERSION1
#include <atomic>

using namespace std;

template <typename T, int Size>
class RingBuffer
    {
    private:
     T buffer[Size];
     std::atomic<int> head, tail;
     std::atomic<int> C;
     
     int NextHead( int n)
         { 
         if(n==(Size-1)) C++;
	 return (n+1)%Size; 
	 }

     int NextTail( int n)
         { 
         if(n==(Size-1)) C--;
	 return (n+1)%Size; 
	 }

    public:
     RingBuffer() : head(0), tail(0) {}

    bool Push(const T& value)
       {
       int H = head.load(memory_order_relaxed);
       if(H == tail.load(memory_order_acquire) && C.load(memory_order_relaxed)) 
	    return false;
       buffer[H] = value;
       head.store(NextHead(H), memory_order_release);
       return true;
       }

    bool Pop(T& value)
       {
       int _T = tail.load(memory_order_relaxed);
       if(_T == head.load(memory_order_acquire)&& !C.load(memory_order_relaxed))
	    return false;
       value = buffer[_T];
       tail.store(NextTail(_T), memory_order_release); 
       return true;
       }

    void Insert(const T& value)
       { while(Push(value)==false); }

    void Extract(T& value)
       { while(Pop(value)==false); }
   };

#endif

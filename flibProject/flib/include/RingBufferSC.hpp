// AUTHOR: Victor Alessandrini, 2015
// VATH libray,in book "Shared Memory Application
// Programming"
// ***********************************************
//
// RingBufferSC class: same as RingBuffer, but
// using the strongest memory consistency model,
// becuse some versions of GNU compiler do not
// fully implement the weaker memory order
// options
//
// This is a memory buffer connecting one producer 
// and one consumer thread.
//
// Does not work for several producers and/or
// consumers.
// -----------------------------------------------

#ifndef RING_BUFFER_SC
#define RING_BUFFER_SC
#include <atomic>

using namespace std;

template <typename T, int Size>
class RingBufferSC
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
     RingBufferSC() : head(0), tail(0) {}

    bool Push(const T& value)
       {
       int H = head.load(memory_order_seq_cst);
       if(H == tail.load(memory_order_seq_cst) && C.load(memory_order_seq_cst)) 
	    return false;
       buffer[H] = value;
       head.store(NextHead(H), memory_order_seq_cst);
       return true;
       }

    bool Pop(T& value)
       {
       int tl = tail.load(memory_order_seq_cst);
       if(tl == head.load(memory_order_seq_cst)&& !C.load(memory_order_seq_cst))
	    return false;
       value = buffer[tl];
       tail.store(NextTail(tl), memory_order_seq_cst); 
       return true;
       }

    void Insert(const T& value)
       { while(Push(value)==false); }

    void Extract(T& value)
       { while(Pop(value)==false); }
   };

#endif

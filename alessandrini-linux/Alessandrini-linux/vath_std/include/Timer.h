// AUTHOR: Victor Alessandrini, 2015
// VATH libray,in book "Shared Memory Application
// Programming"
/* =======================================================
 * Timer.h
 * Puts a thread in an uncoditional wait for a number of
 * miliseconds.
 * ======================================================*/
#ifndef TIMER_H
#define TIMER_H

#include <thread>
#include <chrono>

class Timer
    {        
    public:
     Timer(){}
     ~Timer(){}

	 void Wait(long time_wait)
        {
		std::chrono::milliseconds ms(time_wait);
		std::this_thread::sleep_for(ms);
        }
    }; 
	
#endif

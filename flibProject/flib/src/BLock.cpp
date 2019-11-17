// **************************************
// Copyright (c) 2011 Victor Alessandrini
// All rights reserved.
// **************************************
/****************************************************
 * BLock.C
 * C++11 implementation of Boolean Lock facility
 * Timeouts are coded in seconds or in milliseconds.
 *
 * NOTE: the timed_wait call on a condition
 * variable returns:
 *  - "false" if the call returns because the timed
 *    wait is elapsed
 *  - "true" otherwise
 **************************************************/

#include <BLock.hpp>
#include <iostream>
#include <chrono>

/* Constructor
   =========== */

BLock::BLock(bool init_val) : state(init_val) {}
	
BLock::BLock()
    {
    state = false;
    }
	
BLock::~BLock() {  }

	
/* Accessor functions
   ================== */

void BLock::SetState(bool val)
    {
    std::unique_lock<std::mutex> lock(bmutex);
    state = val;	
    }

bool BLock::GetState()
    {
    bool flag;
       {
       std::unique_lock<std::mutex> lock(bmutex);
       flag = state;	
       }
    return flag;
    }

int BLock::Wait_Until(bool S, long timeout)
    {
    int retval = 1;

	//std::cout << "\n Entering BLock wait with state " << state << std::endl;

    if(timeout==0L)      // unlimited wait
       {
       std::unique_lock<std::mutex> lock(bmutex);
	   while (state == !S)
	       {
		   bcond.wait(lock);
	       }
        }
    else                 // timed wait
        {
        auto const limit_date = std::chrono::steady_clock::now() +
	                       std::chrono::milliseconds(timeout);
        std::unique_lock<std::mutex> lock(bmutex);
        while(state==!S)
           {
           if( bcond.wait_until(lock, limit_date)== std::cv_status::timeout)
	           {
               retval = 0;
               break;
               }
            }
        }
    return retval;
    }


void BLock::Set_And_Notify(bool st)
    {
    std::unique_lock<std::mutex> lock(bmutex);
    state = st;	
	//std::cout << "\n BLock: Notifying " << state << std::endl;
    bcond.notify_one();
    }

void BLock::Set_And_Notify_All(bool st)
    {
    std::unique_lock<std::mutex> lock(bmutex);
    state = st;	
	//std::cout << "\n BLock: notifying " << state << std::endl;
    bcond.notify_all();
    }

/*********************************************************/

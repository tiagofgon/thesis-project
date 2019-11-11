// **************************************
// Copyright (c) 2011 Victor Alessandrini
// All rights reserved.
// **************************************
/*****************************************************************
 * BLock.C
 * Implementation of Boolean Lock facility
 * In my Linux Red Hat platform, the error ETIMEDOUT is not
 * recognized. Therefore, I forced the error in the "timeout.c"
 * code and foud it to be 110.
 * Pre-processor selects if timeouts are coded in seconds or in
 * miliseconds.
 * **************************************************************/

#include <BLock.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <errors.h>

#ifdef RED_HAT_7_1
#define ETIMEDOUT  110
#endif

/* Constructor
   =========== */

BLock::BLock(bool init_val)
    {
    int status;
    // Initialize data items
    state = init_val;
    status = pthread_mutex_init( &(this->block), NULL);
    if(status) err_abort(status, "Mutex initialization");
    status = pthread_cond_init( &(this->bcond), NULL);
    if(status) err_abort(status, "CV initialization");
    }
	
BLock::BLock()
    {
    int status;
    // Initialize data items
    state = false;
    status = pthread_mutex_init( &(this->block), NULL);
    if(status) err_abort(status, "Mutex initialization");
    status = pthread_cond_init( &(this->bcond), NULL);
    if(status) err_abort(status, "CV initialization");
    }
	
BLock::~BLock() {  }

	
/* Accessor functions
   ================== */

void BLock::SetState(bool val)
    {
    int status;

    status = pthread_mutex_lock( &(this->block) );
    if(status) err_abort(status, "Mutex lock");
    state = val;	
    status = pthread_mutex_unlock( &(this->block) );
    if(status) err_abort(status, "Mutex unlock");
    }

bool BLock::GetState()
    {
    int status;
    bool flag;

    status = pthread_mutex_lock( &block );
    if(status) err_abort(status, "Mutex lock");
    flag = state;	
    status = pthread_mutex_unlock( &block );
    if(status) err_abort(status, "Mutex unlock");
    return flag;
    }
	
   /* Wait function.
      Return value: 
      1 for an unlimited wait 
      0 for a timed wait in which time expired
      1 for timed wait in which time did not expire
      =============================================*/

int BLock::Wait_Until(bool st, long time_wait)
    {
    int status, retval;
    int sec, msec;
    struct timeval now;
    struct timezone tz;
    struct timespec timeout;

    status = pthread_mutex_lock( &block );
    if(status) err_abort(status, "Mutex lock");

    if( time_wait == 0L)    /* here, wait is unlimited */
        {
        while( state != st)
            pthread_cond_wait( &bcond, &block);
	    
        status = pthread_mutex_unlock( &block );
        if(status) err_abort(status, "Mutex unlock");
        return 1;
        }

    /* Here, we need to handle a timed wait */

    gettimeofday(&now, &tz);

    #ifdef SECONDS
    timeout.tv_sec = now.tv_sec + time_wait;
    timeout.tv_nsec = now.tv_usec * 1000;
    #else
    sec = time_wait/1000;
    msec = time_wait % 1000;
	
    timeout.tv_sec = now.tv_sec + sec;
    timeout.tv_nsec = now.tv_usec * 1000 + msec * 1000000;
	
    sec = timeout.tv_nsec / 1000000000;
    if(sec)
        {
        timeout.tv_sec += sec;
        timeout.tv_nsec %= 1000000000;
        }
    #endif

    // ------------------------------------------------------------------
    // The timed wait function below returns ETIMEDOUT=110 if the wait is
    // timed out, or 1 if the condition has been notified
    // ------------------------------------------------------------------
    while( (state != st) && status != ETIMEDOUT)
        status = pthread_cond_timedwait( &bcond, &block, &timeout);
    if(status == ETIMEDOUT) retval = 0;
    else retval = 1;

    status = pthread_mutex_unlock( &block );
    if(status) err_abort(status, "Mutex unlock");
    return retval;
    }
	

void BLock::Set_And_Notify(bool st)
    {
    int status;

    status = pthread_mutex_lock( &(this->block) );
    if(status) err_abort(status, "Mutex lock");
    // ------------------------------------------
    state = st;	
    pthread_cond_signal( &bcond );
    // ------------------------------------------
    status = pthread_mutex_unlock( &(this->block) );
    if(status) err_abort(status, "Mutex unlock");
    }


void BLock::Set_And_Notify_All(bool st)
    {
    int status;
    status = pthread_mutex_lock( &(this->block) );
    if(status) err_abort(status, "Mutex lock");
    //------------------------------------------
    state = st;	
    pthread_cond_broadcast( &bcond );
    // -----------------------------------------
    status = pthread_mutex_unlock( &(this->block) );
    if(status) err_abort(status, "Mutex unlock");
    }


/*********************************************************/

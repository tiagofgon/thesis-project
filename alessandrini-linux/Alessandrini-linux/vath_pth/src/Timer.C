// **************************************
// Copyright (c) 2011 Victor Alessandrini
// All rights reserved.
// **************************************
/* ==============================================================
 * Timer.C
 * Implementation of a Timer facility, that puts a thread to
 * wait for a predetermined number of miliseconds.
 * In my Linux Red Hat platform, the error ETIMEDOUT is not
 * recognized. Therefore, I forced the error in the "timeout.c"
 * code and foud it to be 110.
 * Timeouts are coded in miliseconds.
 * =============================================================*/

#include <Timer.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <errors.h>

#define RED_HAT_9
#ifdef RED_HAT_9
#define ETIMEDOUT  110
#endif

/* ------------
 * Constructor
 * ------------*/

Timer::Timer()
	{
	int status;
         
        // Next, initialize mutex and condition variable fields 
	// ---------------------------------------------------
	 
	status = pthread_mutex_init( &lock, NULL);
	if(status) err_abort(status, "Mutex init");
	status = pthread_cond_init( &cond, NULL);
	if(status) err_abort(status, "CV init");

	/* ----------------------------------------------------
	 * The mutex is needed to be able to call the wait on
         * condition, but in this case there is no predicate and
	 * the mutex protects nothing. It can be locked once and
         * for all (because pthread_cond_wait() must be called
         * with a locked mutex)
	 * ---------------------------------------------------*/
	
	status = pthread_mutex_lock( &lock);
	if(status) err_abort(status, "Mutex lock");
	}
	
	
Timer::~Timer()
	{
        int status;

	/* -------------------------------------------
	 * Unlock the mutex locked by the constructor 
	 * ------------------------------------------*/
	 	
	status = pthread_mutex_unlock( &lock);
	if(status) err_abort(status, "Mutex unlock");

	/* ------------------------------------
         * Destroy mutex and condition variable 
	 * -----------------------------------*/
	 
	status = pthread_mutex_destroy( &lock);
	if(status) err_abort(status, "Mutex destroy");
	status = pthread_cond_destroy( &cond);
	if(status) err_abort(status, "CV destroy");
	}

/* ------------------------
 * The basic wait function
 * ------------------------*/ 

void Timer::Wait(long time_wait)
	{
	int status, retval;
	int sec, msec;
	struct timeval now;
	struct timezone tz;
	struct timespec timeout;


	if( time_wait == 0L) return;   /* here, no wait */

	/* ------------------------------------
	 * Here, we need to handle a timed wait 
         * -----------------------------------*/

	gettimeofday(&now, &tz);

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

	pthread_cond_timedwait( &cond, &lock, &timeout);
	}
    
	

/*********************************************************/

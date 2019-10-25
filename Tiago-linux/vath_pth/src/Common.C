// Copyright 2011 Victor Alessandrini
// All rights reserved.
// ----------------------------------
// Implementation of simple threads interface, with implicit
// error handling. The purpose is to avoid including explicit
// error handling in every funcion call
// ---------------------------------------------------------- 

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

/* ----------------------------------------------
 * Wrapper for error handling in pthread_create()
 * ---------------------------------------------*/
 void PthreadCreate( pthread_t *thread,  pthread_attr_t *attr,
                     void *(*thread_fct)(void *), void *arg )
     { 
     int status;
     status = pthread_create(thread, attr, thread_fct, arg);
     if(status)
        {
        printf("\n Error in pthread_create: aborting\n");
        exit(0);
        }
     }


/* --------------------------------------------
 * Wrapper for error handling in pthread_join()
 * --------------------------------------------*/
 void PthreadJoin(pthread_t thread, void **data) 
     {
     int status;
     status = pthread_join(thread, data);
     if(status)
        {
        printf("\n Error in join: aborting\n");
        exit(0);
        }
     }



/* --------------------------------------------------
 * Wrapper for error handling in pthread_mutex_lock()
 * -------------------------------------------------*/
 void Pthread_Mutex_Unlock(pthread_mutex_t *mutex) 
     {
     int status;
     status = pthread_mutex_unlock(mutex);
     if(status)
        {
        printf("\n Error in mutex unlock: aborting\n");
        exit(0);
        }
     }

/* ---------------------------------------------------
 * Wrapper for error handling in pthread_mutex_unlock()
 * ---------------------------------------------------*/
 void Pthread_Mutex_Lock(pthread_mutex_t *mutex) 
     {
     int status;
     status = pthread_mutex_lock(mutex);
     if(status)
        {
        printf("\n Error in mutex lock: aborting\n");
        exit(0);
        }
     }

 void Pthread_Mutex_LockBis(pthread_mutex_t *mutex, const char *str) 
     {
     int status;
     status = pthread_mutex_lock(mutex);
     if(status)
        {
        printf("\n Error in mutex lock, function %s\n", str);
        exit(0);
        }
     }


/* -------------------------------------------------
 * Wrapper for error handling in pthread_cond_wait()
 * -------------------------------------------------*/
 void Pthread_Cond_Wait(pthread_cond_t *cond, 
                             pthread_mutex_t *mutex,
                             bool predicate) 
     {
     int status;
     while(!predicate)
        {
        status = pthread_cond_wait(cond, mutex);
        if(status)
           {
           printf("\n Error in condition wait: aborting\n");
           exit(0);
           }
        }
     }


/* ---------------------------------------------------
 * Wrapper for error handling in pthread_cond_signal()
 * ---------------------------------------------------*/
 void Pthread_Cond_Signal(pthread_cond_t *cond) 
     {
     int status;
     status = pthread_cond_signal(cond);
     if(status)
        {
        printf("\n Error in condition signal: aborting\n");
        exit(0);
        }
     }
 

/* ---------------------------------------------------
 * Wrapper for error handling in pthread_cond_signal()
 * ---------------------------------------------------*/
 void Pthread_Cond_Broadcast(pthread_cond_t *cond) 
     {
     int status;
     status = pthread_cond_broadcast(cond);
     if(status)
        {
        printf("\n Error in condition broadcast: aborting\n");
        exit(0);
        }
     }



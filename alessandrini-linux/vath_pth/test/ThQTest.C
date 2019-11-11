// **************************************
// Copyright (c) 2011 Victor Alessandrini
// All rights reserved.
// **************************************
/* ------------------------------------------------------------------
 * TGAUSS.C
 * This code uses a pipelined architecture to compute random numbers
 * with Gaussian distribution.
 * Thread A produces random numbers with uniform distribution
 * Thread B transforms them into Gaussian randoms
 * The main thread uses them for a statistical analysis.
 *
 * There is an interesting issue in this example. The algorithm in
 * Thread B discards sometimes data coming from Thread A. This means
 * that we do not know a priori how much data Thread A needs to 
 * produce. Therefore, we need a reliable mechanism for cancelling
 * threads.
 *
 * According to Buttenhof, threads waiting on a condition are cancelled
 * unless the cancellation point is explicitly overruled. Therefore,
 * cancelling the threads executing Adds and Removes should not be
 * a problem.
 *
 * NOTE:
 * The naive code has a race condition may occur when threads are 
 * cancelled, because it may happen that:
 * Thread A fills first, is cancelled and leaves behind a locked
 * mutex.
 * Thread B can no longer remove from A, and there is a deadlock.
 * In this code, without cleanup handlers, it is crucial that the
 * main thread waits for the two queue to fill before requesting
 * cancellation.
 *
 * Even in this case, with cleanup handlers, we had deadlocks. 
 * Analysis of flow shows that A is full but that B is not. Why?
 * --------------------------------------------------------------*/

#include <stdio.h> 
#include <Rand.h> 
#include <ThQueue.h>
#include <math.h>

pthread_t thread_A, thread_B;
ThQueue<double> *QA, *QB;
int Nmax;
const double PI = 3.1415926535;

void Cleaner(void *P)
    {
    ThQueue<double> *G = (ThQueue<double> *) P; 
    G->mutex_release();
    printf("\n Cleaner executed");
    }

void *thA(void *arg)       /* first thread function */
	{
	double d, f;
        int cancel_type;
        //pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &cancel_type);
        pthread_cleanup_push(Cleaner, (void *)&QA);

        Rand R;	
	while(1)
		{
		pthread_testcancel();
		d = R.draw();  /* get random in [0,1]*/
		f = 2.0*d - 1.0;    /*    map to [-1, 1]  */
		QA->add(f);
                printf("\n Thread A size = %d", QA->queue_size());
		}
        pthread_cleanup_pop(0);
	return NULL;
	}


void *thB(void *arg)       /* second thread function */
	{
	double x, y, s, fac;
        int cancel_type;
        //pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &cancel_type);
        pthread_cleanup_push(Cleaner, (void *)&QB);
	while(1)
		{
		pthread_testcancel();
		do
		   {
		   x = QA->remove();
                   printf("\n                                   ");
                   printf("Removed from A, size = %d", QA->queue_size());
                   y = QA->remove();
                   printf("\n                                   ");
                   printf("Removed from A, size = %d", QA->queue_size());
		   s = x*x + y*y;
		   } while(s >= 1.0 || s == 0.0);
		
		fac = sqrt(-2.0*log(s)/s);
		x *= fac;
		y *= fac;
                QB->add(x);
                QB->add(y);
                printf("\n                                   ");
                printf("Thread B added, size = %d", QB->queue_size());
		}
        pthread_cleanup_pop(0);
	return NULL;
	}

int main(int argc, char **argv)
	{
	int i, status, nc;
	double d;
 
	if(argc != 3)
	   {
	   printf("\n Wrong command line : usage t_dqueue  nc N");
	   printf("\n where N is the number of doubles produced\n");
	   printf("\n and nc is the capacity of the queue\n");
	   exit(0);
	   }
	
        nc   = atoi(argv[1]);
	Nmax = atoi(argv[2]);

        /* Construct queues */

        QA = new ThQueue<double>(nc);
        QB = new ThQueue<double>(nc);

	/* Start producer threads */

	status = pthread_create(&thread_A, NULL, thA, NULL);
	if(status) error_exit("Thread creation");
	status = pthread_create(&thread_B, NULL, thB, NULL);
	if(status) error_exit("Thread creation");

	/* start getting pipelined data from dqueue QB */
    
	for(i=0; i<Nmax; i++)
		{
		d = QB->remove();
                //printf("\n                                                             ");
		//printf("\n%d  %g", i, d);
		}
	//sleep(1);
	status = pthread_cancel(thread_A);
	if(status) error_exit("Thread A cancel");
	status = pthread_cancel(thread_B);
	if(status) error_exit("Thread B cancel");

        printf("\n Cancellation requests posted\n");
	
	status = pthread_join(thread_A, NULL);
	if(status) error_exit("Thread A join");
	status = pthread_join(thread_B, NULL);
	if(status) error_exit("Thread B join");
	printf("\n Producer threads joined - exiting\n");

        delete QA;
        delete QB;

	return 0;
	} 

// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
/* TSBarrier.C
 *
 * This example tests the SpBarrier class, based on busy waits and
 * spin mutexes. This class implements a busy wait at the barrier
 * synchronization point, using the spin mutex class SpinLock.
 *
 * SpinLock, in turn, encapsulates the native spin lock mutex in
 * Pthreads, and uses custom, atomic based spin mutexes in C++11
 * and Windows.
 * =============================================================*/

#include <iostream>
#include <stdlib.h>
#include <Timer.h>S
#include <SPool.h>
#include <SpBarrier.h>

SPool     *TS;
SpBarrier *B;

void th_fct(void *arg)
    {
    Timer T;
    T.Wait(500);
    std::cout << "\n From thread: before barrier  " << std::endl;
    B->Wait();
    std::cout << "\n From thread : after first barrier " << std::endl;
    B->Wait();
    std::cout << "\n From thread : after second barrier " << std::endl;
    }

int main(int argc, char **argv)
    {
    int nTh;
    if(argc==1) nTh=2;
    else nTh = atoi(argv[1]);

    TS = new SPool(nTh);
    B = new SpBarrier(nTh);

    TS->Dispatch(th_fct, NULL);
    TS->WaitForIdle();
    return 0;
    }
	

/*********************************************************/

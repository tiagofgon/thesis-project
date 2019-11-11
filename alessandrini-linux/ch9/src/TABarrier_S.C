// AUTHOR: Victor Alessandrini, 2015
// Example in book "Shared Memory Application
// Programming"
// ******************************************
/* TABarrier.C
 * 
 * This example tests the atomic barrier ABarrier class. This service
 * relies on native atomic and thread local services, and there is no
 * Pthreads implementation. It is available in C++11 and Windows (in
 * principle, see below).
 *
 * In Pthreads, use the TBarrier class: same thing, based on portable
 * TBB atomic and thread local services.
 *
 * NOTICE: This is correct, portable code. But there may be compiler
 * limitations. This example works correctly with the GNU 4.8 Linux
 * compiler or higher. It does not work with the Visual Studio 2013 
 * compiler (incomplete support of thread_local storage)
 * ===========================================================*/

#include <iostream>
#include <stdlib.h>
#include <Timer.h>
#include <SPool.h>
#include <ABarrier.h>

SPool     *TS;
ABarrier  *B;

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
    B = new ABarrier(nTh);

    TS->Dispatch(th_fct, NULL);
    TS->WaitForIdle();
    return 0;
    }
	

/*********************************************************/

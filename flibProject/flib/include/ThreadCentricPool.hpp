/* ---------------------------------------------------------------------------
                           Made by Tiago Gonçalves - 2019
 --------------------------------------------------------------------------*/

//  ThreadCentricPool.h
//  Very lightweight thread pool specialized 
//  for SPMD parallel codes
// *****************************************
#ifndef ThreadCentricPool_H
#define ThreadCentricPool_H

#include <thread>
#include <mutex>
#include <atomic>
#include "BkBarrier.hpp"
    
class ThreadCentricPool
    {
    private:
    
     //pool characteristics
     //-------------------- 
     int                 nThreads;
     bool                shut;
	 std::atomic<bool>   cancel_flag;

     // current task
     // ------------
     void                   (*fct)(void *);
     void                   *arg;

     // pool state 
     // ----------
     std::thread::id     *th_id;
     std::thread         **WT;
        
     // pool synchronization 
     // --------------------
     std::mutex         pMutex;
     BkBarrier          *BlBarrier;
 
     void JoinThreads();    

    public:

     ThreadCentricPool(int Nth, double d = 0.0);
     ~ThreadCentricPool();

     // Basic management
     // ----------------
     void PeerThread();
     void Dispatch(void (*taskfct)(void *), void *arg);
     void WaitForIdle();

     // Utilities
     // ---------
     int  GetRank();                // returns rank of caller thread

     void CancelTeam();
     void SetCancellationPoint();

    //  void ThreadRange(int& beg, int& end);       // work sharing
    //  void ThreadRange(double& beg, double& end);
     std::pair<int,int> schedule_static(int Beg, int End);
     std::pair<int,int> schedule_dynamic(int Beg, int End);
     std::pair<int,int> schedule_guided(int Beg, int End);

     friend void threadFunc(void *P);
    };

#endif

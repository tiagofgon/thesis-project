// **************************************
// Copyright (c) 2011 Victor Alessandrini
// All rights reserved.
// **************************************
// TestRank.C
//
// Testing the GetRank() function in ThPeer
// ----------------------------------------

#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <NPool.h>
#include <Timer.h>
#include <SafeCout.h>

NPool    *NP;
SafeCout SC;

class MyTask: public Task
   {
   public:
    MyTask(): Task() { }  

    void ExecuteTask()
       {
       Timer T;
       int owner_rank = GetOwnerRank();
       int taskID = GetTaskID();
       T.Wait(1000+500*taskID);
       std::ostringstream os;
       os << " Thread " << owner_rank << " running task "
	      << taskID;   SC.Flush(os);
       }
   };

int main(int argc, char *argv[])
   {
   int nTh;
   std::ostringstream os;

   if(argc == 2) nTh = atoi(argv[1]); 
   else nTh = 2;
   std::cout << "\n Testing NPool rank utility " << std::endl;

   NP = new NPool(nTh, 20);
   TaskGroup *TG = new TaskGroup();
   for(int n=1; n<=nTh; ++n)
      {
      MyTask *t = new MyTask();
      TG->Attach(t);
      }
   int groupID = NP->SubmitJob(TG);
   NP->WaitForJob(groupID);
   os << "\n Parallel job is finished "; SC.Flush(os);
   delete NP;
   return 0;
   }




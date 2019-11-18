/* ---------------------------------------------------------------------------
                           Made by Tiago Gonçalves - 2019
 --------------------------------------------------------------------------*/

// Task.cpp
// -----------------------------------------------------------

#include "Task.hpp"
#include <iostream>
#include <functional>
#include <future>
#include <queue>


// -----------------------------------------------------------
// Class Task 
// **********
//
// Any Task object has an internal Booloean lock, on which the
// task waits for its children while the BLock state is "false". 
// Child tasks will decrease its reference count and, if it
// reaches zero, set the state to "true" and notify the change 
// ----------------------------------------------------------

Task::Task() : TGid(0), parent(NULL), refcount(0), 
             rank(0), task_id(0), iswaited(true) 
   { 
   BL = new BLock();     // false by default
   }
   
Task:: ~Task()   // destructor
   {
   delete BL;
   }                


void Task::IncreaseRefcount()
   {
   std::lock_guard<std::mutex> lock(tMutex);
   if (refcount == 0) BL->SetState(false);
   refcount++;
   }
   

void Task::DecreaseRefcount()
   {
   // ----------------------------------------------------
   // Attention: this member function, called by a task, 
   // decreases its refcount and, if zero, releases waiting 
   // threads. Since the notification argument is "true",
   // further wait calls will not wait. This is needed to
   // avoid missing a notification (a wait call coming after
   // the notification)
   //
   // IN THE CODE, THIS FUNCTION IS ALWAYS CALLED BY THE 
   // PARENT TASK, IF ANY. Remember: any task has a pointer
   // to its parent task. 
   // -----------------------------------------------------
   std::lock_guard<std::mutex> lock(tMutex);
   refcount--;
   if (refcount == 0) BL->Set_And_Notify_All(true);
   }


// -----------------------------
// Caller task waits for childs,
// and resets Boolean lock.
// -----------------------------
void Task::WaitForChilds()
   {
   BL->Wait_Until(true, 0L);
   }

// Getters and setters
// -------------------
void Task::SetWaited(bool B)
   {
   iswaited = B;
   }


bool Task::Am_I_Waited()
   {
   bool retval;
      {
      std::lock_guard<std::mutex> lock(tMutex);
      retval = iswaited;
      }      
   return retval;
   }

int Task::GetJobid()
   {
   int retval;
      {
      std::lock_guard<std::mutex> lock(tMutex);
      retval = TGid;
      }
   return retval;
   }
 
void Task::SetJobid(int jid)
   {
   std::lock_guard<std::mutex> lock(tMutex);
   TGid = jid;
   }
 
Task* Task::GetParent()
   {
   Task *retval;
      {
      std::lock_guard<std::mutex> lock(tMutex);
      retval = parent;
      }
   return retval;
   }
 
void Task::SetParent(Task *T)
   {
   std::lock_guard<std::mutex> lock(tMutex);
   parent = T;
   }
 
int Task::GetOwnerRank()
   {
   int retval;
      {
      std::lock_guard<std::mutex> lock(tMutex);
      retval = rank;
      }
   return retval;
   }
 
void Task::SetOwnerRank(int R)
   {
   std::lock_guard<std::mutex> lock(tMutex);
   rank = R;
   }
 
int Task::GetTaskID()
    {
    int retval;
        {
        std::lock_guard<std::mutex> lock(tMutex);
        retval = taskID;
        }
    return retval;
    }

void Task::SetTaskID(int R)
    {
    std::lock_guard<std::mutex> lock(tMutex);
    taskID = R;
    }

BLock* Task::GetBLock()
   {
   BLock *retval;
      {
      std::lock_guard<std::mutex> lock(tMutex);
      retval = BL;
      }
   return retval;
   }
 
void Task::ResetBLock()
   {
   std::lock_guard<std::mutex> lock(tMutex);
   BL->SetState(false);
   }


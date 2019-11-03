/* ---------------------------------------------------------------------------
    Copyright 2014-2015 Victor Alessandrini.  All Rights Reserved.

    This file is part of the software support provided wih the book
    "Shared Mamory Application Programming".

    This code is free software; you can redistribute it and/or modify it under 
    the terms of the GNU General Public License version 2 as published by 
    the Free Software Foundation.

    This software is distributed in the hope that it will be useful, but 
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
    for more details.

    You should have received a copy of the GNU General Public License
    along with Threading Building Blocks; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
 --------------------------------------------------------------------------*/
// Task.C
// -----------------------------------------------------------

#include "Task.h"
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


template < typename Func, typename ... Args>
Task::Task(Func &&    func, Args && ...args) : TGid(0), parent(NULL), refcount(0), 
             rank(0), task_id(0), iswaited(true){
   BL = new BLock();     // false by default
   
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


// // custom task factory
//     template <typename Func, typename ... Args,
//         typename Rtrn=typename std::result_of<Func(Args...)>::type>

//     auto Task::make_task(
//         Func &&    func,
//         Args && ...args) -> std::packaged_task<Rtrn(void)> {

//         auto aux = std::bind(std::forward<Func>(func),
//                              std::forward<Args>(args)...);
 
//         return std::packaged_task<Rtrn(void)>(aux);
//     }

    

// template <
//         typename     Func,
//         typename ... Args,
//         typename Pair=Func(Args...),
//         typename Rtrn=typename std::result_of<Pair>::type>
//     auto Task::enqueue(
//         Func &&     func,
//         Args && ... args) -> std::future<Rtrn> {

//         // create the task, get the future
//         // and wrap task in a shared pointer
//         auto task = make_task(func, args...);
//         auto future = task.get_future();
//         auto task_ptr = std::make_shared<decltype(task)>(std::move(task));

//             // wrap the task in a generic void
//             // function void -> void
//             auto payload = [task_ptr] ( ) -> void {
//                 // basically call task()
//                 task_ptr->operator()();
//             };

//             // append the task to the queue
//             tasks.emplace(payload);
        

//         return future;
//     }

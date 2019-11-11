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
// LATEST VERSION
// **************************************

#include <Task.h>
#include <Common.h>
#include <errors.h>

Task::Task() : TGid(0), parent(NULL), refcount(0), 
             rank(0), iswaited(true) 
    { 
    int status = pthread_mutex_init( &task_mutex, NULL);
    if(status != 0) err_abort(status, "Mutex Init");
    BL = new BLock(false);
    }
   
Task::~Task()   // destructor
    {
    pthread_mutex_destroy(&task_mutex);
    delete BL;
    }                

// Getters and setters
// -------------------
void Task::SetWaited(bool B)
    {
    Pthread_Mutex_LockBis(&task_mutex, "SetWaited()");
    iswaited = B;
    Pthread_Mutex_Unlock(&task_mutex);
    }

bool Task::Am_I_Waited()
    {
    bool retval;
    Pthread_Mutex_LockBis(&task_mutex, "Am_I_Waited()");
    retval = iswaited;
    Pthread_Mutex_Unlock(&task_mutex);
    return retval;
    }

int Task::GetJobid()
    {
    int retval;
    Pthread_Mutex_LockBis(&task_mutex, "GetJobid()");
    retval = TGid;
    Pthread_Mutex_Unlock(&task_mutex);
    return retval;
    }
 
void Task::SetJobid(int jid)
    {
    Pthread_Mutex_LockBis(&task_mutex, "SetJobid()");
    TGid = jid;
    Pthread_Mutex_Unlock(&task_mutex);
    }
 
int Task::GetTaskID()
    {
    int retval;
    Pthread_Mutex_LockBis(&task_mutex, "GetTaskID()");
    retval = taskID;
    Pthread_Mutex_Unlock(&task_mutex);
    return retval;
    }

void Task::SetTaskID(int R)
    {
    Pthread_Mutex_LockBis(&task_mutex, "SetTaskID()");
    taskID = R;
    Pthread_Mutex_Unlock(&task_mutex);
    }
 
Task* Task::GetParent(const char *str)
    {
    Task *retval;
    Pthread_Mutex_LockBis(&task_mutex, str);
    retval = parent;
    Pthread_Mutex_Unlock(&task_mutex);
    return retval;
    }
 
void Task::SetParent(Task *T)
    {
    Pthread_Mutex_LockBis(&task_mutex, "SetParent()");
    parent = T;
    Pthread_Mutex_Unlock(&task_mutex);
    }
 
int Task::GetOwnerRank()
    {
    int retval;
    Pthread_Mutex_LockBis(&task_mutex, "GetOwnerRank()");
    retval = rank;
    Pthread_Mutex_Unlock(&task_mutex);
    return retval;
    }
 
void Task::SetOwnerRank(int R)
    {
    Pthread_Mutex_LockBis(&task_mutex, "SetOwnerRank()");
    rank = R;
    Pthread_Mutex_Unlock(&task_mutex);
    }
 
BLock* Task::GetBLock()
    {
    BLock *retval;
    Pthread_Mutex_LockBis(&task_mutex, "GetBLock()");
    retval = BL;
    Pthread_Mutex_Unlock(&task_mutex);
    return retval;
    }
 
void Task::ResetBLock()
    {
    Pthread_Mutex_LockBis(&task_mutex, "ResetBLock()");
    BL->SetState(false);
    Pthread_Mutex_Unlock(&task_mutex);
    }


void Task::IncreaseRefcount()
   {
   Pthread_Mutex_LockBis(&task_mutex, "IncreaseRefcount()");
   refcount++;
   Pthread_Mutex_Unlock(&task_mutex);
   }
   
void Task::DecreaseRefcount()
   {
   // ----------------------------------------------------
   // Attention: this member function, called by a task, 
   // decreases its refcount and, if zero, sets its own 
   // Boolean Lock to true and notifies the change. 
   //
   // IN THE CODE, THIS FUNCTION IS ALWAYS CALLED BY THE 
   // PARENT TASK, IF ANY. Remember: any task has a pointer
   // to its parent task. 
   // ------------------------------------------------------
   
   Pthread_Mutex_LockBis(&task_mutex, "DecreaseRefCount()");
   // --------------------------------
   refcount--;
   if(refcount==0) BL->Set_And_Notify(true);
   // --------------------------------
   Pthread_Mutex_Unlock(&task_mutex);
   }

// -----------------------------
// Caller task waits for childs,
// and resets Boolean lock.
// -----------------------------
void Task::WaitForChilds()
   {
   BL->Wait_Until(true, 0);
   BL->SetState(false);
   }


/* ---------------------------------------------------------------------------
                           Made by Tiago Gonçalves - 2019
 --------------------------------------------------------------------------*/

// Task.hpp
//
// This class is part of the TaskCentricPool utility. The Task class 
// describes a task. 
//
// Task objects carry a lot of information:
// - identifier of job to which the task belongs
// - pointer to parent task: valid pointer if parent task is
//   run by worker thread (explicit task in OpenMP language), 
//   null pointer if it is run by external thread (implicit
//   task in OMP language)
// - number of child tasks to this task
// - bool that informs if a parent task is waiting for this 
//   one   

// New features:
// - Eliminates task hierarchy
// - taskFunction wich store the task function to execute in the TaskCentricPool
// - make_task member function: receives as argument a template function and their arguments. 
//   Return a packaged_task with the function and their arguments wrapped
// - insertTask member function: This function, called on user program,
//   receives as argument a template function and their arguments. With make_task member function
//   create a task and return a future connected to that task.
// *********************************************************

#ifndef MYTASK_H
#define MYTASK_H

#include "BLock.hpp"
#include <mutex>
#include <future>
#include <list>
#include <cstdint>
#include <future>
#include <functional>

class Task {
   private:
   //-----
    std::mutex        tMutex;     // task mutex
    int               TGid;       // ID of job this task belongs
    Task              *parent;    // ponter to parent task. NULL by default
    int               refcount;   // number of my childs
    int               rank;       // rank of owner thread
    BLock             *BL;
    bool              iswaited;   // is my parent waiting for me?
    int               taskID;     // a unique integer, used for debugging only
    


   public:
   //----
    std::function<void(void)> taskFunction;
    int task_id;       // used for debugging only

    Task();            // constructor

    ~Task();   // destructor                
    
    void IncreaseRefcount();           // increases my refcount
    void DecreaseRefcount();           // decreases parent refcount
    void WaitForChilds();

    void SetWaited(bool B);
    bool Am_I_Waited();

    int GetJobid();
    void SetJobid(int jid);

    Task *GetParent();
    void SetParent(Task *T);

    int GetOwnerRank();
    void SetOwnerRank(int R);

    int  GetTaskID();
    void SetTaskID(int R);

    BLock *GetBLock();
    void ResetBLock(); 

    // custom task factory
    template <typename Func, typename ... Args,
        typename Rtrn=typename std::result_of<Func(Args...)>::type>
    auto make_task(
        Func &&    func,
        Args && ...args) -> std::packaged_task<Rtrn(void)> {

        auto aux = std::bind(std::forward<Func>(func),
                             std::forward<Args>(args)...);
 
        return std::packaged_task<Rtrn(void)>(aux);
    }

    template <
        typename     Func,
        typename ... Args,
        typename Pair=Func(Args...),
        typename Rtrn=typename std::result_of<Pair>::type>
    auto insertTask(
        Func &&     func,
        Args && ... args) -> std::future<Rtrn> {

        // create the task, get the future
        // and wrap task in a shared pointer
        auto task = make_task(func, args...);
        auto future = task.get_future();
        auto task_ptr = std::make_shared<decltype(task)>(std::move(task));

            // wrap the task in a generic void
            // function void -> void
            auto payload = [task_ptr] ( ) -> void {
                // basically call task()
                task_ptr->operator()();
            };

            // append the task to taskFunction
            taskFunction=std::move(payload);
        

        return future;
    }

   };

// ********************************************************
// The TaskGroup structure encapsulates a group of pointers 
// to tasks as a list of Task objects
// ****************************************************

struct TaskGroup
   {
   std::list<Task*> LT;
   TaskGroup() {}
   ~TaskGroup() { LT.clear(); }
   void Attach(Task *T) { LT.push_back(T); }
   void Clear() { LT.clear(); }
   int  Size() { return LT.size(); }
   };

#endif
/////////////////////////////////////////////////////////////

# Master thesis project
The main objective of this project is reserch the parallelism and performance that C++ can offer.

# Librarys
- alessandrini-linux
- thread_pool
- project

## alessandrini-linux
A relatively small, high-level library presented in the book **Shared Memory Application Programming Concepts and strategies in multicore application programming**, called vath, written by the book author **Victor Alessandrini**. This library proposes some high-level, easy-to-use utilities in the form of C++ classes that encapsulate the low-level primitives provided by the basic libraries. The main propose use of this library on this project is to take advantage of the SPool and NPool thread pool classes proposed, implementing, respectively, a thread-centric or a task-centric programming environment.


## thread_pool
In the book **Parallel Programming: Concepts and Practice**, was presented a thread pool. This thread pool maintains a list of tasks with arbitrary return values and arguments for two reasons. First, neither C++11 nor C++14 features an out-of-the-box thread pool and thus you have to reimplement it anyway in aforementioned examples. Second, a correctly working thread pool exhibits non-trivial synchronization and signaling mechanism that are ideally suited to demonstrate the benefits of condition variables, as well as futures and promises in an educational context. The proposed implementation is heavily inspired by Jakob Progsch’s and Václav Zeman’s thread pool library and has been rewritten with a focus on readability and educational clarity.

## project
This library is inspired on Vath and Thread Pool libraries and is my contribution to open source community.

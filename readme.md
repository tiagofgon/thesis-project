# C++ project

# Directories
- alessandrini-linux
- threadPool
- flibProject

## flibProject
In this folder contains a library that is based in vath library, with some new features implemented, some of them based on Thread Pool library. The goal is reserch the parallelism and performance that C++ can offer to initiate my master's thesis.

## alessandrini-linux
Folder that contains the code developed with the book **Shared Memory Application Programming Concepts and strategies in multicore application programming**, whose author is **Victor Alessandrini**. The library, called vath, is a relatively small, high-level library and proposes some high-level, easy-to-use utilities in the form of C++ classes that encapsulate the low-level primitives provided by the basic libraries. The main propose use of this library is illustrate a thread-centric and task-centric programming environment.


## threadPool
This folder contains a simple library that implement a thread pool that maintains a list of tasks with arbitrary return values and arguments. Was developed with the book **Parallel Programming: Concepts and Practice**, whose authors are **Bertil Schmidt**, **Jorge González-Domínguez**, **Christian Hundt** and **Moritz Schlarb**. This thread pool library exhibits non-trivial synchronization and signaling mechanism that are ideally suited to
demonstrate the benefits of condition variables, as well as futures and promises in an educational context.

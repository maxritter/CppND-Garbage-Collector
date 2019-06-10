# CppND-Garbage-Collector
In this project, a garbage collector is implemented in C++, similar to the functionality of a smart pointer.

The garbage collector works by creating a new pointer type that provides built-in support for garbage collection based on reference counting. The garbage collector is single-threaded, which means that it is quite portable and does not rely upon (or make assumptions about) the execution environment.



## Compilation

`g++ -o gc.out main.cpp -std=c++1y -Wall` 



## Execution

`./gc.out` 


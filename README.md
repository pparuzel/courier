# courier

[![Build status](https://github.com/reconndev/courier/workflows/build/badge.svg)](https://github.com/reconndev/courier/actions)

A simple, lightweight **C++17 event bus**/dispatcher.
It allows communication between components through subscription and events. A function is attached to a particular event, user posts an event and then all subscribed functions are called with this specific event.

Event dispatchers are particularly popular in game development. Usually, physics engines send events about collisions between entities so that the users of the engine can subscribe, to handle them however they want without coupling game code with the physics engine code. Events may contain mutable data. `courier` permits any type to be sent as long as it is registered in the dispatcher.

## Table of contents

* [Benchmark](#benchmark)
    * [Description](#description)
    * [EnTT vs courier](#entt-vs-courier)
* [Tested compilers](#tested-compilers)
* [Example](#example)
* [Compiling tests and examples](#compiling-tests-and-examples)
    * [Requirements](#requirements)
    * [Steps](#steps)
* [License](#license)

## Benchmark

### Description
`courier` and other programs were compiled with `-O3` to squeeze out the best out of the optimizer. The benchmarking tests were perfomed on an 8-Core Intel Core i9 16 GB.

### EnTT vs courier

Results were **rounded** to the 5th decimal place. Times are represented in **milliseconds**.

#### Three events (max sizeof = 4 bytes) sent thousands of times

| Iterations | Mean time `courier`  | Mean time `EnTT` | Std Dev `courier` | Std Dev `EnTT` |
| ---------: |:--------------------:| :---------------:| :--------------:  | :-------------:|
| 20k        | 0.02ms               | 0.23ms           | 0.00ms            | 0.06ms         |
| 50k        | 0.05ms               | 0.52ms           | 0.00ms            | 0.06ms         |
| 100k       | 0.09ms               | 1.05ms           | 0.02ms            | 0.13ms         |
| 200k       | 0.18ms               | 2.11ms           | 0.02ms            | 0.20ms         |
| 500k       | 0.46ms               | 5.28ms           | 0.09ms            | 0.31ms         |

This shows about **10x improvement** over `EnTT` for small events.

#### Three events (max sizeof = 80 bytes) sent thousands of times

| Iterations | Mean time `courier` | Mean time `EnTT` | Std Dev `courier` | Std Dev `EnTT` |
| ---------: |:-------------------:| :---------------:| :---------------: | :------------: |
| 20k        | 0.13ms              | 0.34ms           | 0.02ms            | 0.04ms         |
| 50k        | 0.32ms              | 0.81ms           | 0.02ms            | 0.08ms         |
| 100k       | 0.69ms              | 1.72ms           | 0.14ms            | 0.09ms         |
| 200k       | 1.38ms              | 3.42ms           | 0.19ms            | 0.30ms         |
| 500k       | 3.64ms              | 9.01ms           | 0.54ms            | 0.58ms         |

This shows about **2.5x improvement** over `EnTT` for large events.

## Tested compilers
* Linux CentOS x86-64, clang version 5.0.1
* Linux CentOS x86-64, GCC version 7.3.1
* MacOS, Apple clang version 11.0.3 
* MacOS, LLVM clang version 9.0.1
* MacOS, GCC version 9.2.0

## Example
```cpp

/* Create some event classes */
struct HelloEvent {
    // ...
};

/* Another event class */
struct GoodbyeEvent {
    std::string name;
};

/* Dispatcher will not known this event for the sake of an example */
struct IgnoredEvent {};

/* This function will be attached to HelloEvent, later */
void on_hello(HelloEvent& event) {
    // ...
}

/* Define a dispatcher with all subscribable events */
courier::dispatcher<HelloEvent, GoodbyeEvent> dispatcher;  // handle HelloEvent and GoodbyeEvent
/* Attach functions to events */
dispatcher.add<HelloEvent>(&on_hello);
dispatcher.add<HelloEvent>([](auto&&...) { std::cout << "Hello!\n"; });
dispatcher.add<GoodbyeEvent>([](auto&& event) { std::cout << "Bye " << event.name << '\n'; });
/* Send events */
dispatcher.post<HelloEvent>();  // calls on_hello function and then prints "Hello!"
dispatcher.post<GoodbyeEvent>("Joe");  // prints "Bye Joe"
dispatcher.post<IgnoredEvent>();  // ignored because dispatcher does not know IgnoredEvent
                                  // also, runtime warning is displayed
dispatcher.add<IgnoredEvent>();  // error: dispatcher cannot subscribe to an unspecified event
```

**Removing attached functions**
```cpp
auto some_event_handler = dispatcher.add<SomeEvent>(&on_some_event);
dispatcher.remove(some_event_handler);
```

**Attaching member functions**
```cpp
Widget obj;
dispatcher.add<SomeEvent, &Widget::foo>(obj);
```

## Compiling tests and examples
### Requirements
* CMake (tested on 3.14)
* C++ compiler with support for C++17

### Steps
```shell
# Generate a build/ directory
cmake -B build . -DCMAKE_BUILD_TYPE=Release
# Run generated Makefiles to compile everything
make -C build -j12
```
## License

This project is under MIT License.

Copyright (c) 2020 Pawel Paruzel

> Permission is hereby granted, free of charge, to any person obtaining a copy  
> of this software and associated documentation files (the "Software"), to deal  
> in the Software without restriction, including without limitation the rights  
> to use, copy, modify, merge, publish, distribute, sublicense, and/or sell  
> copies of the Software, and to permit persons to whom the Software is  
> furnished to do so, subject to the following conditions:  
> 
> 
> The above copyright notice and this permission notice shall be included in all  
> copies or substantial portions of the Software.  
> 
> 
> THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  
> IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  
> FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE  
> AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER  
> LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  
> OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  
> SOFTWARE.

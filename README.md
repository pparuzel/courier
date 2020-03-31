# courier

[![Build status](https://github.com/reconndev/courier/workflows/build/badge.svg)](https://github.com/reconndev/courier/actions)

A simple, lightweight **C++17 event bus**/dispatcher.
It allows communication components through subscription and events. A function is attached to a particular event and once a user posts an event, all subscribed functions are called.

Event dispatchers are particularly popular in game development. Usually, physics engines send events about collisions between entities so that the users of the engine can subscribe, to handle them however they want without coupling game code with the physics engine code. Events may contain mutable data. `courier` permits any type to be sent as long as it is registered in the dispatcher.

## Benchmark
### EnTT vs courier

Results were **rounded** to the 5th decimal place. Times are represented in **seconds**.

#### Three events (max sizeof = 4 bytes) sent thousands of times

| Iterations | Mean time | Std Dev  | Mean time (EnTT) | Std Dev (EnTT) |
| ---------: |:---------:| :-------:| :--------------: | :-------------:|
| 20k        | 0.00002s  | 0.00000s | 0.00023s         | 0.00006s       |
| 50k        | 0.00005s  | 0.00000s | 0.00052s         | 0.00006s       |
| 100k       | 0.00009s  | 0.00002s | 0.00105s         | 0.00013s       |
| 200k       | 0.00018s  | 0.00002s | 0.00211s         | 0.00020s       |
| 500k       | 0.00046s  | 0.00009s | 0.00528s         | 0.00031s       |

#### Three events (max sizeof = 80 bytes) sent thousands of times
| Iterations | Mean time | Std Dev  | Mean time (EnTT) | Std Dev (EnTT) |
| ---------: |:---------:| :-------:| :--------------: | :------------: |
| 20k        | 0.00013s  | 0.00002s | 0.00034s         | 0.00004s       |
| 50k        | 0.00032s  | 0.00002s | 0.00081s         | 0.00008s       |
| 100k       | 0.00069s  | 0.00014s | 0.00172s         | 0.00009s       |
| 200k       | 0.00138s  | 0.00019s | 0.00342s         | 0.00030s       |
| 500k       | 0.00364s  | 0.00054s | 0.00901s         | 0.00058s       |

## Tested on compilers
* Linux CentOS x86-64, clang version 5.0.1
* Linux CentOS x86-64, GCC version 7.3.1
* MacOS, Apple clang version 11.0.3 
* MacOS, LLVM clang version 9.0.1
* MacOS, GCC version 9.2.0

## Usage
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

### Removing attached functions
```cpp
auto some_event_handler = dispatcher.add<SomeEvent>(&on_some_event);
dispatcher.remove(some_event_handler);
```

### Attaching member functions
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
cmake -B build .
make -C build -j12
```
## License

This project is under MIT License.

Copyright (c) 2020 Pawel Paruzel

Permission is hereby granted, free of charge, to any person obtaining a copy  
of this software and associated documentation files (the "Software"), to deal  
in the Software without restriction, including without limitation the rights  
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell  
copies of the Software, and to permit persons to whom the Software is  
furnished to do so, subject to the following conditions:  


The above copyright notice and this permission notice shall be included in all  
copies or substantial portions of the Software.  


THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,  
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE  
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER  
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,  
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE  
SOFTWARE.

# courier

[![Build status](https://github.com/reconndev/courier/workflows/build/badge.svg)](https://github.com/reconndev/courier/actions)

A simple, lightweight C++17 event bus/dispatcher.
It allows communication components through subscription and events. A function is attached to a particular event and once a user posts an event, all subscribed functions are called.

Event dispatchers are particularly popular in game development. Physics engine send an event about a collision between two entities and users of the engine can subscribe, to handle this event however they want without coupling with the physics engine code. Events may contain mutable data, courier permits any type to be sent as long as it is registered in the dispatcher.

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

MIT License

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

# courier

[![Build status](https://github.com/reconndev/courier/workflows/build/badge.svg)](https://github.com/reconndev/courier/actions)

A simple, lightweight event bus/dispatcher written in Modern C++.
It allows communication components through subscription and events. A function is attached to a particular event and once a user posts an event, all subscribed functions are called.

Event dispatchers are particularly popular in game development. Physics engine send an event about a collision between two entities and users of the engine can subscribe, to handle this event however they want without coupling with the physics engine code. Events may contain mutable data, courier permits any type to be sent as long as it is registered in the dispatcher.

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
* CMake (tested on 3.15)
* C++ compiler with support for C++2a

```shell
cmake -B build .
make -C build -j12
```

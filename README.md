# Courier
A simple event dispatcher written in Modern C++.

## Usage
```cpp
struct HelloEvent {
    // ...
};

struct GoodbyeEvent {
    std::string name;
};

struct IgnoredEvent {};

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
dispatcher.post<IgnoredEvent>();  // ignored because dispatcher does not know that event
                                  // runtime warning is displayed
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

cmake_minimum_required(VERSION 3.15)
project(EventBus)

# Set variable pointing at the files required by other CMakeLists.txt
set(EVENTBUS_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Compile examples
add_subdirectory(examples)

# Compile tests
add_subdirectory(tests)

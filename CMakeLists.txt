cmake_minimum_required(VERSION 3.14)
project(Courier)

# Set variable pointing at the files required by other CMakeLists.txt
set(COURIER_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Compile examples
add_subdirectory(examples)

# Compile tests
add_subdirectory(tests)

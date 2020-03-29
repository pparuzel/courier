cmake_minimum_required(VERSION 3.15)
project(Courier_Example)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT DEFINED COURIER_DIR)
    message(WARNING "CMake variable COURIER_DIR is not defined")
endif()

include_directories(${COURIER_DIR}/include)

add_executable(example1 example1.cpp ${COURIER_DIR}/include/ebus/ebus.hpp)

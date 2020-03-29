cmake_minimum_required(VERSION 3.15)
project(EventBusExample)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT DEFINED EVENTBUS_DIR)
    message(WARNING "CMake variable EVENTBUS_DIR is not defined")
endif()

include_directories(${EVENTBUS_DIR}/include)

add_executable(example1 example1.cpp ${EVENTBUS_DIR}/include/ebus/ebus.hpp)

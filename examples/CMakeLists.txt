cmake_minimum_required(VERSION 3.15)
project(EventBusExample)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT DEFINED EBUS_DIR)
    message(WARNING "CMake variable EBUS_DIR is not defined")
endif()

include_directories(${EBUS_DIR}/include)

add_executable(example1 example1.cpp ${EBUS_DIR}/include/ebus/ebus.hpp)

cmake_minimum_required(VERSION 3.15)
project(EBus_Tests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT DEFINED EBUS_DIR)
    message(WARNING "CMake variable EBUS_DIR is not defined")
endif()

include_directories(${EBUS_DIR}/include)
include_directories(Catch2/single_include/catch2)

add_executable(testcase_1 basic_functionality.cpp)

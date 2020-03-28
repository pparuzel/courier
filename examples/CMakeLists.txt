cmake_minimum_required(VERSION 3.15)
project(EMS)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${EMS_DIR}/include)

add_executable(ems_test test1.cpp)

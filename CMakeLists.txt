cmake_minimum_required(VERSION 3.15)
project(EMS)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(EMS ems_test.cpp ems.hpp)

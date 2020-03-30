cmake_minimum_required(VERSION 3.14)
project(Courier_Example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT DEFINED COURIER_DIR)
    message(WARNING "CMake variable COURIER_DIR is not defined")
endif()

find_package(Threads REQUIRED)
include_directories(${COURIER_DIR}/include)
set(COURIER_HEADER_FILE ${COURIER_DIR}/include/courier/courier.hpp)

add_executable(example1 example1.cpp ${COURER_HEADER_FILE})
add_executable(example2 example2.cpp ${COURER_HEADER_FILE})
target_link_libraries(example2 Threads::Threads)

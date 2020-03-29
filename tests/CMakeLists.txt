cmake_minimum_required(VERSION 3.15)
project(Courier_Tests)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

if (NOT DEFINED COURIER_DIR)
    message(WARNING "CMake variable COURIER_DIR is not defined")
endif ()

add_custom_command(OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/Catch2
        COMMAND git clone https://github.com/catchorg/Catch2.git
        WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        COMMENT "Downloading Catch2 C++ tests framework"
        VERBATIM
        )

add_custom_target(DownloadCatch2 ALL
        DEPENDS Catch2
        )

include_directories(${COURIER_DIR}/include)
include_directories(Catch2/single_include/catch2)

add_executable(testcase_1 basic_functionality.cpp)
add_dependencies(testcase_1 DownloadCatch2)

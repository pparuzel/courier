cmake_minimum_required(VERSION 3.14)
project(Courier_Tests)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

if (NOT DEFINED COURIER_DIR)
    message(WARNING "CMake variable COURIER_DIR is not defined")
endif ()

# Add Catch2 library
set(CATCH_INCLUDE_DIR Catch2/single_include/catch2)
add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE ${CATCH_INCLUDE_DIR})

# Add courier library
set(COURIER_INCLUDE_DIR ${COURIER_DIR}/include)
add_library(Courier INTERFACE)
target_include_directories(Courier INTERFACE ${COURIER_INCLUDE_DIR})

# Define test executables
add_executable(BasicTests basic.cpp)
add_executable(Benchmark benchmark/benchmark.cpp)

# Linking stage
target_link_libraries(BasicTests Catch Courier)
target_link_libraries(Benchmark Catch Courier)

# Make sure Catch2 is pre-downloaded
add_custom_command(OUTPUT Catch2/single_include
        COMMAND git submodule foreach git reset --hard
        COMMAND git submodule update --init -- ${CMAKE_CURRENT_SOURCE_DIR}/Catch2
        WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
        COMMENT "Downloading Catch2 C++ tests framework"
        VERBATIM
        )
add_custom_target(DownloadCatch2 ALL
        DEPENDS Catch2/single_include
        )
add_dependencies(Catch DownloadCatch2)

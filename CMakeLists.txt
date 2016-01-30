cmake_minimum_required(VERSION 2.8)
PROJECT(2DPhysics)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${CMAKE_SOURCE_DIR})

if(WIN32)
add_definitions(-W2 -DWITH_WINDOWS)
else()
add_definitions(-W -Wall -ansi)
ENDIF()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_definitions(-Wno-c++11-extensions)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_definitions(-std=c++11)
endif()

add_subdirectory(math)
add_subdirectory(geometry)
add_subdirectory(simulation)
add_subdirectory(testbed)

cmake_minimum_required(VERSION 2.8)
PROJECT(2DPhysics)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/math)
include_directories(${CMAKE_SOURCE_DIR}/geometry)
include_directories(${CMAKE_SOURCE_DIR}/simulation)

add_definitions(-W -Wall -ansi -Wno-c++11-extensions)

add_subdirectory(math)
add_subdirectory(geometry)
add_subdirectory(simulation)

cmake_minimum_required(VERSION 2.8)
PROJECT(2DPhysics)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

add_definitions(-W -Wall -ansi -Wc++11-extensions)

add_subdirectory(math)
add_subdirectory(geometry)

cmake_minimum_required(VERSION 2.8)
PROJECT(geometry)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/math)

add_definitions(-W -Wall -ansi)

set( LIB_SOURCE src/circle.cpp)
set( LIB_HEADERS baseshape.h circle.h)

add_library(${PROJECT_NAME} ${LIB_SOURCE} ${LIB_HEADERS})

add_subdirectory(tests)

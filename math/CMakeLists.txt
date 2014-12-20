cmake_minimum_required(VERSION 2.8)
PROJECT(math)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_definitions(-W -Wall -ansi)

set( LIB_SOURCE src/vector2.cpp)
set( LIB_HEADERS gmath.h vector2.h transform.h)

add_library(${PROJECT_NAME} ${LIB_SOURCE} ${LIB_HEADERS})

add_subdirectory(tests)

cmake_minimum_required(VERSION 2.8)
PROJECT(simulation)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

set( LIB_SOURCE src/actor.cpp src/world.cpp)
set( LIB_HEADERS actor.h world.h)

add_library(${PROJECT_NAME} ${LIB_SOURCE} ${LIB_HEADERS})

add_subdirectory(tests)

cmake_minimum_required(VERSION 2.8)
PROJECT(simulation)

include_directories(${CMAKE_SOURCE_DIR}/math)
include_directories(${CMAKE_SOURCE_DIR}/geometry)
include_directories(${CMAKE_SOURCE_DIR}/simulation)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

set( LIB_SOURCE src/actor.cpp src/world.cpp src/simshape.cpp src/solver.cpp)
set( LIB_HEADERS actor.h world.h simshape.h solver.h)

add_library(${PROJECT_NAME} ${LIB_SOURCE} ${LIB_HEADERS})

add_subdirectory(tests)

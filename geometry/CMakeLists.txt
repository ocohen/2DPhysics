cmake_minimum_required(VERSION 2.8)
PROJECT(geometry)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

set( LIB_SOURCE src/circleshape.cpp src/rectangleshape.cpp src/convexpolygon.cpp src/baseshape.cpp)
set( LIB_HEADERS baseshape.h circleshape.h rectangleshape.h convexpolygon.h shapeoverlap.h)

include_directories(${CMAKE_SOURCE_DIR}/math)
include_directories(${CMAKE_SOURCE_DIR}/geometry)

add_library(${PROJECT_NAME} ${LIB_SOURCE} ${LIB_HEADERS})

add_subdirectory(tests)

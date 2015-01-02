cmake_minimum_required(VERSION 2.8)
PROJECT(math)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

set( LIB_SOURCE src/vector2.cpp src/transform.cpp)
set( LIB_HEADERS gmath.h vector2.h transform.h aabb.h)

include_directories(${CMAKE_SOURCE_DIR}/math)

add_library(${PROJECT_NAME} ${LIB_SOURCE} ${LIB_HEADERS})

add_subdirectory(tests)

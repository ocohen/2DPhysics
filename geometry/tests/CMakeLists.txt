cmake_minimum_required(VERSION 2.8)
PROJECT(geometry_tests)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/math)
include_directories(${CMAKE_SOURCE_DIR}/geometry)
include_directories(${CMAKE_SOURCE_DIR}/extern)

set(SOURCE run.cpp)

add_executable(${PROJECT_NAME} ${SOURCE})
target_link_libraries(${PROJECT_NAME} geometry math)

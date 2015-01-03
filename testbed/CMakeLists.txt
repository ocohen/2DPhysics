cmake_minimum_required(VERSION 2.8)
PROJECT(testbed)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${CMAKE_SOURCE_DIR}/math)
include_directories(${CMAKE_SOURCE_DIR}/geometry)
include_directories(${CMAKE_SOURCE_DIR}/simulation)
include_directories(${CMAKE_SOURCE_DIR}/extern)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set( TESTBED_SOURCE src/test.cpp src/shapeoverlap.cpp src/renderer.cpp src/forces.cpp src/impulses.cpp)
set( TESTBED_HEADERS test.h renderer.h)

#OpenGL
find_package(OpenGL)
include_directories(${OPENGL_INCLUDE_DIR})

#SDL
find_package(SDL2)
include_directories(${SDL2_INCLUDE_DIR})

add_executable(${PROJECT_NAME} ${TESTBED_SOURCE} ${TESTBED_HEADERS})
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARY} ${SDL_MAIN} ${SDL2_LIBRARY} simulation geometry math)

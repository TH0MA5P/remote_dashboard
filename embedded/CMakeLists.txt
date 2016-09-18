cmake_minimum_required(VERSION 2.8)

SET( PROJ_NAME      "EMBEDDED" )

# Change this to change the application version.
set (VERSION_SOFT_MAJOR 0)
set (VERSION_SOFT_MINOR 0)

add_definitions(-DVERSION_MAJOR=${VERSION_SOFT_MAJOR})
add_definitions(-DVERSION_MINOR=${VERSION_SOFT_MINOR})
add_definitions(-DENABLE_LOG=1)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -std=gnu99 -O0 -g")

file (GLOB PROJ_SOURCES ./src/*.c)

set(CMAKE_BUILD_DIRECTORY "build")

project(${PROJ_NAME} C CXX)

include_directories(./inc ../common)

add_executable(${PROJ_NAME} ${PROJ_SOURCES})

target_link_libraries( ${PROJ_NAME} pthread )


# setting minimum version
cmake_minimum_required(VERSION 3.16)

# setting project's name
project(ces35_lab02)

# setting compilation flags
SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -pthread")

# setting output executable directory
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_HOME_DIRECTORY}/bin )

# adding /src
add_subdirectory(src)

cmake_minimum_required(VERSION 2.8.11)

project(Neuro)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../)
if(WIN32)
    set(CMAKE_CXX_FLAGS "-std=c++11 -W3")
else()
    set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -g")
endif()
add_subdirectory(src)

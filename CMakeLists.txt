cmake_minimum_required(VERSION 3.1...3.12)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()


project(Neuro)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../)
if(WIN32)
    add_compile_options(/W3)
else()
    add_compile_options(-Wall)
endif()
add_subdirectory(src)

enable_testing()

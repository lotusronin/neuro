find_package(LLVM REQUIRED)

#if( NOT LLVM_FOUND )
#    message(FATAL_ERROR "Could not find LLVM.")
#endif()

include_directories(${LLVM_INCLUDE_DIRS})



include_directories(${Neuro_SOURCE_DIR}/include)
set(MY_SOURCES main.cpp lexer.cpp parser.cpp astnode.cpp
    asttransforms.cpp symboltable.cpp
    irgen.cpp ccodegen.cpp linker.cpp graphDotGenerator.cpp
    errors.cpp arrayview.cpp)

if (WIN32)
    list(APPEND MY_SOURCES win32_linker.cpp)
elseif (UNIX)
    list(APPEND MY_SOURCES linux_linker.cpp)
endif ()

add_executable(Neuro ${MY_SOURCES})

llvm_map_components_to_libnames(llvm_libs core native)
target_link_libraries(Neuro ${llvm_libs})

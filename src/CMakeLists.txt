find_package(LLVM REQUIRED)

#if( NOT LLVM_FOUND )
#    message(FATAL_ERROR "Could not find LLVM.")
#endif()

include_directories(${LLVM_INCLUDE_DIRS})



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
target_compile_features(Neuro PUBLIC cxx_std_11)
set_target_properties(Neuro PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(Neuro PUBLIC "${Neuro_SOURCE_DIR}/include/")

llvm_map_components_to_libnames(llvm_libs core native)
target_link_libraries(Neuro ${llvm_libs})


#run cppcheck if it is found
find_program(CPPCHECK_PROG cppcheck)
if(CPPCHECK_PROG)
    list(APPEND CPPCHECK_PROG
        "--inconclusive"
        "--enable=performance"
        "--enable=portability")
    set_target_properties(Neuro PROPERTIES CXX_CPPCHECK "${CPPCHECK_PROG}")
endif()


#run cpplint if it is found
find_program(CPPLINT_PROG cpplint)
if(CPPLINT_PROG)
    list(APPEND CPPLINT_PROG
        "--filter=-whitespace,-legal,-build")
    set_target_properties(Neuro PROPERTIES CXX_CPPLINT "${CPPLINT_PROG}")
endif()

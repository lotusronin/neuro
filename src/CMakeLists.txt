find_package(LLVM REQUIRED)

#if( NOT LLVM_FOUND )
#    message(FATAL_ERROR "Could not find LLVM.")
#endif()

include_directories(${LLVM_INCLUDE_DIRS})


add_library(Catch INTERFACE)
target_include_directories(Catch INTERFACE "${Neuro_SOURCE_DIR}/include/catch/single_include/catch2/")

set(MY_TEST_SOURCES test_catch_main.cpp lexer.cpp astnode.cpp
    test_asttransforms.cpp symboltable.cpp
    errors.cpp arrayview.cpp typeinfo.cpp)

set(MY_SOURCES main.cpp lexer.cpp parser.cpp astnode.cpp
    asttransforms.cpp symboltable.cpp
    irgen.cpp ccodegen.cpp linker.cpp graphDotGenerator.cpp
    errors.cpp arrayview.cpp typeinfo.cpp)

if (WIN32)
    list(APPEND MY_SOURCES win32_linker.cpp)
elseif (UNIX)
    list(APPEND MY_SOURCES linux_linker.cpp)
endif ()

add_executable(Neuro ${MY_SOURCES})
target_compile_features(Neuro PUBLIC cxx_std_17)
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
        "--quiet"
        "--filter=-whitespace,-legal,-build")
    set_target_properties(Neuro PROPERTIES CXX_CPPLINT "${CPPLINT_PROG}")
endif()

#run catch
option(RUN_TESTS "Run unit tests" ON)
if(RUN_TESTS)
    add_executable(test_Neuro ${MY_TEST_SOURCES})
    target_link_libraries(test_Neuro Catch)
    target_compile_features(test_Neuro PUBLIC cxx_std_17)
    set_target_properties(test_Neuro PROPERTIES CXX_EXTENSIONS OFF)
    target_include_directories(test_Neuro PUBLIC "${Neuro_SOURCE_DIR}/include/")
    target_include_directories(test_Neuro PUBLIC "${Neuro_SOURCE_DIR}/src/")

    add_test(NAME TestNeuro COMMAND test_Neuro)
endif()

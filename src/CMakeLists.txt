include_directories(${Neuro_SOURCE_DIR}/include)
set(MY_SOURCES main.cpp lexer.cpp regexes.cpp parser.cpp)

add_executable(Neuro ${MY_SOURCES})

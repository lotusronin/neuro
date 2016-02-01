include_directories(${Neuro_SOURCE_DIR}/include)
set(MY_SOURCES main.cpp lexer.cpp regexes.cpp parser.cpp astnode.cpp compileunitnode.cpp
    prototypenode.cpp programnode.cpp paramsnode.cpp funcdefnode.cpp blocknode.cpp forloopnode.cpp
    whileloopnode.cpp returnnode.cpp typenode.cpp deferstmtnode.cpp loopstmtnode.cpp
    ifnode.cpp elsenode.cpp funccallnode.cpp varnode.cpp vardecnode.cpp vardecassignnode.cpp)

add_executable(Neuro ${MY_SOURCES})

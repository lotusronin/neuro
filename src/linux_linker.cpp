#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>

std::string getDefaultExeFilename(std::string in) {
    if(in.size() == 0) {
        return "";
    }

    int size = in.size();
    if(in.substr(size-4,4) == ".nro") {
        in[size-1] = 'e';
        in[size-2] = 'x';
        in[size-3] = 'e';
    } else {
        in = in+".exe";
    }

    return in;
}

std::string linux_getLinkerCmd(std::string file) {
    //TODO(marcus): Can we just call ld directly? Maybe use ldd?
    std::string linker = "cc ";
    std::string objs = file+".o ";
    std::string exefile = getDefaultExeFilename(file);
    std::string output = "-o "+exefile+" ";

    //std::string systemlibs = "/usr/lib64/crt1.o /usr/lib64/crti.o /usr/lib64/crtn.o -lc ";
    //std::string other_options = " ";

    //std::string ret = linker + output + objs + systemlibs + other_options;
    std::string ret = linker + output + objs;
    return ret;
}

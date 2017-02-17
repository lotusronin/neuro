#include <vector>
#include <string>
#include <cstdlib>
#include <iostream>

std::string win32_getLinkerCmd(std::string file);

std::string getLinkerCmd(std::string filename) {
#if _WIN32
    return win32_getLinkerCmd(filename);
#else
    //TODO(marcus): get this working for Linux
    return "";
#endif
}

void linkFiles(std::vector<std::string>& files) {
    std::string cmd = getLinkerCmd(files[0]);
    //system(("link.exe "+files[0]+".o "+cmd).c_str());
    system(cmd.c_str());
}

void linkFile(std::string file) {
    //auto t = ("link.exe "+file+".o "+cmd);
    std::string cmd = getLinkerCmd(file);
    std::cout << cmd << '\n';
    system(cmd.c_str());
}

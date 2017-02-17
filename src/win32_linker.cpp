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

std::string win32_getLinkerCmd(std::string file) {
    std::string linker = "link.exe ";
    std::string objs = file+".o ";
    std::string exefile = getDefaultExeFilename(file);
    std::string output = "/OUT:\""+exefile+"\" ";
    //"\"test.exe\" ";
    //std::string systemlibs = "/NXCOMPAT /DYNAMICBASE \"kernel32.lib\" \"user32.lib\" \"gdi32.lib\" \"winspool.lib\" \"shell32.lib\" \"ole32.lib\" \"oleaut32.lib\" \"uuid.lib\" \"comdlg32.lib\" \"advapi32.lib\" \"psapi.lib\" ";
    std::string libpaths = "\"/LIBPATH:C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.10150.0\\ucrt\\x64\" /LIBPATH:\"C:\\Program Files (x86)\\Microsoft Visual Studio 14.0\\VC/lib\\amd64\" /LIBPATH:\"C:\\Program Files (x86)\\Windows Kits\\8.1\\Lib\\winv6.3\\um\\x64\" /NXCOMPAT /DYNAMICBASE ";
    std::string systemlibs = "\"kernel32.lib\" \"user32.lib\" \"gdi32.lib\" \"winspool.lib\" \"shell32.lib\" \"ole32.lib\" \"oleaut32.lib\" \"uuid.lib\" \"comdlg32.lib\" \"advapi32.lib\" \"psapi.lib\" \"msvcrt.lib\" ";
    std::string other_options = "/DEBUG /MACHINE:X64 /OPT:NOREF /INCREMENTAL:NO /SUBSYSTEM:CONSOLE /OPT:ICF /ERRORREPORT:PROMPT /NOLOGO";

    /*
     */
    std::string ret = linker + objs + output + libpaths + systemlibs + other_options;
    return ret;
    //return output+libpaths+systemlibs+other_options;
}

#include <iostream>
#include <string>
#include <vector>
#include "lexer.h"


int main(int argc, char** argv) {
    std::cout << "Welcome to the neuro compiler.\n";

    if(argc < 2) {
        std::cout << "Usage:\n  Neuro <inputfiles>\n";
        return 0;
    } else {
        std::vector<std::string> cmd_args;
        for(int i = 1; i < argc; i++) {
            cmd_args.push_back(argv[i]);
        }

        for (auto f : cmd_args) {
            LexerTarget target1 = LexerTarget(f);
            std::string tok;
            while(tok.compare("EOF") != 0) {
                tok = target1.lex();
            }
        }
        return 0;
    }
    //Should never reach!
    std::cerr << "Well this is awkward...\n";
    return -1;
}

#include <iostream>
#include <string>
#include <vector>
#include "lexer.h"
#include "tokens.h"
#include "parser.h"

bool debug_lexer;

void badargspass() {
    std::cout << "Usage:\n  Neuro <inputfiles>\n";
    std::cout << "Options:\n  -dbgl = lexer debug output\n";
    std::cout << "Coming:\n  -dbgp = parser debug output\n  -dbga = all debug output\n";
}

void parseargs(int argc, char** argv, std::vector<std::string>& cmd_args) {
    std::string dbgl("-dbgl");
    for(int i = 1; i < argc; i++) {
        if(dbgl.compare(argv[i]) == 0) {
            debug_lexer = true;
        } else {
            cmd_args.push_back(argv[i]);
        }
    }
}

int main(int argc, char** argv) {
    std::cout << "Welcome to the neuro compiler.\n";

    if(argc < 2) {
        badargspass();
        return 0;
    } else {
        std::vector<std::string> sources;
        parseargs(argc, argv, sources);
        if(sources.size() == 0) {
            badargspass();
            return 0;
        }

        std::cout << "Beginning Lexing...\ndebug: " << debug_lexer << "\n";

        for (auto f : sources) {
            LexerTarget target1 = LexerTarget(f, debug_lexer);
            Parser parser = Parser(&target1);
            parser.parse();

            /*
            Token tok;
            while(tok.type != TokenType::eof) {
                tok = target1.lex();
                //std::cout << "Token: " << tok.token << " at (" << tok.line << "," << tok.col << ")\n";
            }*/
        }
        return 0;
    }
    //Should never reach!
    std::cerr << "Well this is awkward...\n";
    return -1;
}

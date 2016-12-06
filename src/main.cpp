#include <iostream>
#include <string>
#include <vector>
#include "lexer.h"
#include "tokens.h"
#include "parser.h"
#include "astnode.h"
#include "asttransforms.h"
#include "irgen.h"
#include "ccodegen.h"

bool debug_lexer;
bool debug_parser;
bool semantic_error = false;

void badargspass() {
    std::cout << "Usage:\n  Neuro <inputfiles>\n";
    std::cout << "Options:\n  -dbgl = lexer debug output\n  -dbgp = print AST generated by parser\n";
    std::cout << "Coming:\n  -dbga = all debug output\n";
}

void parseargs(int argc, char** argv, std::vector<std::string>& cmd_args) {
    std::string dbgl("-dbgl");
    std::string dbgp("-dbgp");
    for(int i = 1; i < argc; i++) {
        if(dbgl.compare(argv[i]) == 0) {
            debug_lexer = true;
        } else if(dbgp.compare(argv[i]) == 0) {
            debug_parser = true;
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
            AstNode* ast = parser.parse();
            
            std::cout << "Beginning AST transformations and Semantic analysis\n";
            collapseExpressionChains(ast);
            checkContinueBreak(ast, 0);
            fixOperatorAssociativity(ast);
            populateSymbolTableFunctions(ast);
            variableUseCheck(ast);
            printSymbolTable();
            typeCheckPass(ast);
            //printSymbolTable();
            //decorateAst(ast);
            if(debug_parser) {
                //Generate Dot file for debugging
                std::ofstream dotfileout(target1.targetName()+".dot",std::ofstream::out);
                ast->makeGraph(dotfileout);
                dotfileout.close();
                std::string cmd = "dot -Tpng "+target1.targetName()+".dot -o "+target1.targetName()+".png";
                std::cout << "Running command: " << cmd << "\n";
                system(cmd.c_str());
            }

            if(!semantic_error) {
                std::cout << "Generating IR code\n";
                //Generate IR code
                generateIR(ast);
                std::cout << "IR output:\n";
                dumpIR();

                //writeObj(target1.targetName());
                writeIR(target1.targetName());
            }

            if(false) {
                SymbolTable* s = getSymtab(f);
                const std::vector<SymbolTableEntry*> symtab_entries = getFunctionEntries(s);
                genCFile(f,symtab_entries);
            }

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

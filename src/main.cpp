#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <chrono>
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
bool timedOut = false;
bool outputIR = false;

void badargspass() {
    std::cout << "Usage:\n  Neuro <inputfiles>\n";
    std::cout << "Options:\n  -dbgl = lexer debug output\n  -dbgp = print AST generated by parser\n";
    std::cout << " -t = print timing information\n  -l = output ir\n";
    std::cout << "Coming:\n  -dbga = all debug output\n";
}

void parseargs(int argc, char** argv, std::vector<std::string>& cmd_args) {
    std::string dbgl("-dbgl");
    std::string dbgp("-dbgp");
    std::string t("-t");
    std::string l("-l");
    std::string h("-h");
    for(int i = 1; i < argc; i++) {
        if(dbgl.compare(argv[i]) == 0) {
            debug_lexer = true;
        } else if(dbgp.compare(argv[i]) == 0) {
            debug_parser = true;
        } else if(t.compare(argv[i]) == 0) {
            timedOut = true;
        } else if(l.compare(argv[i]) == 0) {
            outputIR = true;
        } else if(h.compare(argv[i]) == 0) {
            badargspass();
        } else {
            cmd_args.push_back(argv[i]);
        }
    }
}

void linkFile(std::string file);
void makeDotGraph(std::ofstream& outfile, AstNode* ast);

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

        //std::cout << "Sizeof(AstNode) = " << sizeof(AstNode) << '\n';
        //std::cout << "Sizeof(Token) = " << sizeof(Token) << '\n';
        //std::cout << "Sizeof(TypeInfo) = " << sizeof(TypeInfo) << '\n';

        //std::cout << "Beginning Lexing...\ndebug: " << debug_lexer << "\n";

        for (auto f : sources) {
            auto start_total = std::chrono::steady_clock::now();
            LexerTarget target1 = LexerTarget(f, debug_lexer);
            target1.lexFile();
            auto end_lex = std::chrono::steady_clock::now();
            auto diff_lex = end_lex-start_total;
 
            auto start_parse = std::chrono::steady_clock::now();
            Parser parser = Parser(&target1);
            AstNode* ast = parser.parse();
            auto end_parse = std::chrono::steady_clock::now();
            auto diff_parse = end_parse - start_parse;
            
            //std::cout << "Beginning AST transformations and Semantic analysis\n";
            auto start_semantic = std::chrono::steady_clock::now();
            transformAssignments(ast);
            populateTypeList(ast);
            resolveSizeOfs(ast);
            importPrepass(ast);
            populateSymbolTableFunctions(ast);
            semanticPass1(ast);
            //printSymbolTable();
            typeCheckPass(ast);
            //printSymbolTable();
            //decorateAst(ast);
            deferPass(ast);
            auto end_semantic = std::chrono::steady_clock::now();
            auto diff_semantic = end_semantic - start_semantic;
            if(debug_parser) {
                //Generate Dot file for debugging
                std::ofstream dotfileout(target1.targetName()+".dot",std::ofstream::out);
                auto start = std::chrono::steady_clock::now();
                makeDotGraph(dotfileout,ast);
                auto end = std::chrono::steady_clock::now();
                auto diff = end - start;
                std::cout << "Time for make graph: " << std::chrono::duration_cast<std::chrono::milliseconds>(diff).count() << "ms\n";
                dotfileout.close();
                std::string cmd = "dot -Tpng "+target1.targetName()+".dot -o "+target1.targetName()+".png";
                std::cout << "Running command: " << cmd << "\n";
                system(cmd.c_str());
            }

            auto start_ir = std::chrono::steady_clock::now();
            auto start_link = start_ir;
            if(!semantic_error) {
                //std::cout << "Generating IR code\n";
                //Generate IR code
                generateIR(ast);
                if(outputIR) {
                    std::cout << "IR output:\n";
                    dumpIR();
                }
                start_link = std::chrono::steady_clock::now();
                writeObj(target1.targetName());
                linkFile(target1.targetName());
                //writeIR(target1.targetName());
            }
            auto end_total = std::chrono::steady_clock::now();
            auto diff_total = end_total - start_total;
            auto diff_ir = start_link - start_ir;
            auto diff_link = end_total - start_link;
            if(timedOut) {
                auto lex_time = std::chrono::duration_cast<std::chrono::milliseconds>(diff_lex).count();
                auto parse_time = std::chrono::duration_cast<std::chrono::milliseconds>(diff_parse).count();
                auto semantic_time = std::chrono::duration_cast<std::chrono::milliseconds>(diff_semantic).count();
                auto ir_time = std::chrono::duration_cast<std::chrono::milliseconds>(diff_ir).count();
                auto link_time = std::chrono::duration_cast<std::chrono::milliseconds>(diff_link).count();
                auto total_time = std::chrono::duration_cast<std::chrono::milliseconds>(diff_total).count();
                std::cout << "Lex Time: " << lex_time << "ms\n";
                std::cout << "Time for parsing: " << parse_time << "ms\n";
                std::cout << "Time for semantic passes: " << semantic_time << "ms\n";
                if(!semantic_error) {
                    std::cout << "Time for IR generation: " << ir_time << "ms\n";
                    std::cout << "Time for linking: " << link_time << "ms\n";
                }
                std::cout << "Total Time: " << total_time << "ms\n";
                //BinOpNode::printDeleted();
            }

            if(false) {
                SymbolTable* s = getSymtab(f);
                const std::vector<SymbolTableEntry*> symtab_entries = getFunctionEntries(s);
                genCFile(f,symtab_entries);
            }
        }
        return 0;
    }
    //Should never reach!
    std::cerr << "Well this is awkward...\n";
    return -1;
}

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <ctype.h>
#include "lexer.h"

LexerTarget::LexerTarget(std::string name) {
    filename = name;
    content = read_file(name);
    lineNum = 0;
}

LexerTarget::~LexerTarget() {
}

std::string LexerTarget::lex() {
    std::cout << "Lexing!!!!!!\n";

    std::string nonalpha = "{}[]()<>*+-/=.==\"";
    std::string two_char = "/+=-*";
    std::string token = "";
    int comment_depth = 0;
    char last_char = 0;
    
    for (auto st : content) {
        std::cout << "\n" << lineNum << ": " << st << "\n";
        int sub_begin = 0;
        int sub_len = 0;
        int count = 0;
        for(auto& c : st) {
            count++;

            switch(last_char) {
                case '/':
                    if(c == '*') {
                        //std::cout << "In a comment\n";
                        comment_depth++;
                        last_char = 0;
                    }
                    break;
                case '*':
                    if(c == '/') {
                        //std::cout << "Out a comment\n";
                        comment_depth--;
                        last_char = 0;
                        if(comment_depth == 0) {
                            token += st.substr(sub_begin,count);
                            std::cout << token << '\n';
                            token = "";
                            sub_begin = count;
                            sub_len = 0;
                        }
                    }
                    break;
                default:
                    if(last_char != 0)
                        std::cout << last_char << '\n';
                    break;
            }

            if(comment_depth) {
                if(c == '/' || c == '*') {
                    last_char = c;
                }
                continue;
            }
            
            if(nonalpha.find(c,0) != std::string::npos) {
                if(sub_len > 0) {
                    token = st.substr(sub_begin, sub_len);
                    std::cout << token << '\n';
                    token = "";
                    sub_begin += sub_len+2;
                    sub_len = 0;
                }
                
                if(two_char.find(c,0) != std::string::npos) {
                    std::cout << "foo " << c << "\n";
                    last_char = c; 
                    sub_len = 1;
                }
                continue;
            }

            if(isspace(c)) {
                token = st.substr(sub_begin, sub_len);
                std::cout << token << '\n';
                token = "";
                sub_begin += sub_len+1;
                sub_len = 0;
                continue;
            }
            
            sub_len += 1;
        }

        if(comment_depth) {
            token += st.substr(sub_begin)+'\n';
        } else if(sub_len) {
            token = st.substr(sub_begin, sub_len);
            std::cout << token << '\n';
            token = "";
        }

        lineNum++;
    }
}

std::vector<std::string> read_file(const std::string& filename) {
    std::ifstream in(filename);
    std::vector<std::string> content;
    std::string line;
    while(std::getline(in, line)) {
        content.push_back(line);
    }
    
    return content;
}

int main() {
    LexerTarget target1 = LexerTarget("test.txt");
    target1.lex();
    return 0;
}

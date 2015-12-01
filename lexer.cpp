#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <regex>
#include <ctype.h>
#include "lexer.h"

LexerTarget::LexerTarget(std::string name) {
    filename = name;
    content = read_file(name);
    lineNum = 0;
    colNum = 0;
    sub_begin = sub_len = 0;
}

LexerTarget::~LexerTarget() {
}

std::string LexerTarget::lex() {

    std::string nonalpha = "{}[]()<>*+-/=.==\"";
    std::string two_char = "/+=-*";
    std::string token = "";
    int comment_depth;
    char last_char = 0;

    std::string ln = content.at(lineNum);
    
    int i;

    if(comment_depth) {
        return lexcomment();
    }
    
    while(isspace(ln[sub_begin])) {
        sub_begin += 1;
        if(ln.size() <= sub_begin) {
            sub_begin = 0;
            lineNum++;
            ln = content.at(lineNum);
        }
    }

    char c = ln[sub_begin];

    if(nonalpha.find(c,0) != std::string::npos) {
        if(ln.size() > sub_begin+1) {
            if(c == '/' && ln[sub_begin+1] == '*') {
                std::cout << "comment begin \n";
                sub_begin += 1;
                comment_depth++;
            }
        }
    }

    for(i = sub_begin; i < ln.size(); i++,sub_len++) {
        char c = ln[i];

    }

    for (auto st : content) {
        std::cout << "\n" << lineNum << ": " << st << "\n";
        sub_begin = 0;
        sub_len = 0;
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

std::string LexerTarget::lexcomment() {

    return "";
}

std::string LexerTarget::lex(bool b) {
    std::string token = "";
    std::string tokentype = "";

    if(lineNum >= content.size()) {
        return "EOF";
    }
    
    std::string ln = content.at(lineNum);
    if(colNum >= ln.size()) {
        lineNum++;
        colNum = 0;
        if(lineNum >= content.size())
            return "EOF";
        ln = content.at(lineNum);
    }

    while(isspace(ln[colNum])) {
        colNum++;

        //std::cout << "there is a space!!!\n";
        if(colNum >= ln.size()) {
            if(lineNum+1 >= content.size()) {
                return "EOF";
            } else {
                lineNum++;
                ln = content.at(lineNum);
                colNum = 0;
            }
        }
    }
            

    std::regex longest_regex_match;
    int longest_match = 0;

    //std::cout << "colNum = " << colNum << '\n';
    std::cout << ln << '\n';
    for(int c = 0; c < colNum-1; c++) {
        std::cout << '-';
    }
    std::cout << "-^\n";
    for (int i = 0; i < num_regexes; i++) {
        std::smatch tmp;
        std::string remaining = ln.substr(colNum);
        bool matched = std::regex_search(remaining,tmp,regexes[i]);
        if(matched) {
            //std::cout << "Matched for regex: " << i << '\n';
            //std::cout << "Total # of matches: " << tmp.size() << '\n';
        for(int j = 0; j < tmp.size(); j++) {
            //make sure match starts at beginning of last token
            //std::cout << "Match for: " << tmp[j] << " at pos " << tmp.position(j) <<'\n';
            if(tmp.position(j) != 0) continue;

            if(tmp.length(j) > longest_match) {
                longest_match = tmp.length(j);
                longest_regex_match = regexes[i];
            }
        }
        }
    }
    
    token = ln.substr(colNum,longest_match);
    colNum += longest_match;
    
    std::cout << "token: " << token << "\n\n";
    return token;

}

std::vector<std::string> read_file(const std::string& filename) {
    std::ifstream in(filename);
    std::vector<std::string> content;
    std::string line;
    while(std::getline(in, line)) {
        content.push_back(line);
        std::cout << line << '\n';
    }
    
    return content;
}

int main() {
    LexerTarget target1 = LexerTarget("test.txt");
    //target1.lex();
    std::string tok;
    while(tok.compare("EOF") != 0) {
        tok = target1.lex(true);
    }
    return 0;
}

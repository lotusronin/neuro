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
    comment_depth = 0;
}

LexerTarget::~LexerTarget() {
}

void LexerTarget::lexcomment() {
    
    while(comment_depth > 0) {
        //std::cout << "Parsing comment block\n";
        std::smatch match_blk_comment_open;
        std::smatch match_blk_comment_close;
        
        if(lineNum >= content.size()) {
            std::cout << "Comment reaches the end of the file\n";
            //we've reached end of file, stop
            comment_depth = 0;
            return;
        }
    
        std::string ln = content.at(lineNum);
        
        if(colNum >= ln.size()) {
            //std::cout << ln << '\n';
            lineNum++;
            colNum = 0;
            if(lineNum >= content.size()) {
                std::cout << "Comment reaches the end of the file\n";
                comment_depth = 0;
                return;
            }
            ln = content.at(lineNum);
        }
    
        std::string t = ln.substr(colNum);

        if(std::regex_search(t,match_blk_comment_open,std::regex("^/\\*"))) {
            for(int i = 0; i < match_blk_comment_open.size(); i++) {
                if(match_blk_comment_open.position(i) != 0) continue;
                ++comment_depth;
                colNum += 2;
                //std::cout << "Block comment begins. Depth " << comment_depth << "\n";
            }
        } else if(std::regex_search(t,match_blk_comment_close,std::regex("^\\*/"))) {
            for(int i = 0; i < match_blk_comment_close.size(); i++) {
                if(match_blk_comment_close.position(i) != 0) continue;
                --comment_depth;
                colNum += 2;
                //std::cout << "Block comment ends. Depth " << comment_depth << "\n";
            }
        } else {
            ++colNum;
        }
    }
}

std::string LexerTarget::lex() {
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


    std::smatch match_comment;
    std::smatch match_blk_comment;
    std::string t = ln.substr(colNum);

    /*
     * check for block comments
     */
    if(std::regex_search(t,match_blk_comment,std::regex("^/\\*"))) {
        for(int i = 0; i < match_blk_comment.size(); i++) {
            if(match_blk_comment.position(i) != 0) continue;
            ++comment_depth;
            colNum += 2;
            //std::cout << "Block comment begins. Depth " << comment_depth << "\n";
            //std::cout << "OOGIE BOOGIE BOO\n";
            lexcomment();
            return lex();
        }
    }
   

    //std::cout << "colNum = " << colNum << '\n';
    std::cout << ln << '\n';
    if(colNum == 0) {
        std::cout << "^\n";
    } else {
        for(int c = 0; c < colNum-1; c++) {
            std::cout << '-';
        }
        std::cout << "-^\n";
    }


    /*
     * Check for line comments
     */
    if(std::regex_search(t,match_comment,std::regex("^//"))) {
        for(int i = 0; i < match_comment.size(); i++) {
            if(match_comment.position(i) != 0) continue;

            colNum = 0;
            lineNum++;
            if(lineNum >= content.size()) {
                return "EOF";
            }
            ln = content.at(lineNum);
            return lex();
        }
    }

    /*
     * Match other tokens
     */
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
    std::cout << '\n';
    
    return content;
}


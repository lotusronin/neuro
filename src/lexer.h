#ifndef _LEXER_H_
#define _LEXER_H_

#include <vector>
#include <string>
#include <regex>
#include "regexes.h"

class LexerTarget
{
public:
    LexerTarget(std::string name);
    ~LexerTarget();
    std::string lex();
    void lexcomment();

private:
    std::string filename;
    std::vector<std::string> content;
    unsigned int lineNum, colNum;
    int sub_begin, sub_len;
    int comment_depth;
    /* data */
};

std::vector<std::string> read_file(const std::string& filename);
#endif

#ifndef _LEXER_H_
#define _LEXER_H_

#include <vector>
#include <string>
#include <regex>
#include "regexes.h"
#include "tokens.h"

class LexerTarget
{
public:
    LexerTarget(std::string name, bool debug);
    ~LexerTarget();
    Token lex();
    Token peek();
    void lexcomment();
    bool isDebug();

private:
    std::string filename;
    std::vector<std::string> content;
    unsigned int lineNum, colNum;
    int sub_begin, sub_len;
    int comment_depth;
    bool debug_out;
    Token currentTok;
    /* data */
};

std::vector<std::string> read_file(const std::string& filename);
#endif

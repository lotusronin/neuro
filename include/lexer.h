#ifndef _LEXER_H_
#define _LEXER_H_

#include <vector>
#include <string>
#include <regex>
#include "tokens.h"

class LexerTarget
{
public:
    LexerTarget(std::string name, bool debug);
    ~LexerTarget();
    Token lex();
    Token lex_internal();
    void lexFile();
    Token peek();
    Token peekNext();
    void lexcomment();
    bool isDebug();
    std::string targetName();

private:
    std::string filename;
    char* content;
    char* ln;
    std::vector<Token> tokenizedFile;
    unsigned int lineNum, colNum;
    unsigned int f_idx;
    int sub_begin, sub_len;
    int comment_depth;
    bool debug_out;
    unsigned int currentIdx;
    /* data */
};

char* read_file(const std::string& filename);
#endif

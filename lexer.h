#ifndef _LEXER_H_
#define _LEXER_H_

#include <vector>
#include <string>

class LexerTarget
{
public:
    LexerTarget(std::string name);
    ~LexerTarget();
    std::string lex();

private:
    std::string filename;
    std::vector<std::string> content;
    int lineNum;
    //int sub_begin, sub_pos;
    /* data */
};

std::vector<std::string> read_file(const std::string& filename);

#endif

#ifndef _LEXER_H_
#define _LEXER_H_

#include <vector>
#include <string>
#include <regex>

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
    int lineNum, colNum;
    int sub_begin, sub_len;
    int comment_depth;
    /* data */
};

std::vector<std::string> read_file(const std::string& filename);

std::regex regexes[] = { 
    std::regex("fn"),
    std::regex("extern"),
    std::regex("/\\*"),
    std::regex("//.*"),
    std::regex("[a-zA-Z_][a-zA-Z0-9_]*"),
    std::regex("[0-9]+\\.[0-9]*"),
    std::regex("[0-9]+"),
    std::regex("="),    
    std::regex("=="),    
    std::regex("!="),    
    std::regex("<"),    
    std::regex("<="),    
    std::regex(">"),    
    std::regex(">="),    
    std::regex("\\("),    
    std::regex("\\)"),    
    std::regex("\\{"),    
    std::regex("\\}"),    
    std::regex("\\."),    
    std::regex("\\,"),    
    std::regex("\\+"),    
    std::regex("-"),    
    std::regex("\\*"),    
    std::regex("/"),    
    std::regex(";"),    
    std::regex(":"),    
    std::regex("!"),  
    std::regex("\\?")   
};
int num_regexes = sizeof(regexes)/sizeof(std::regex);
#endif

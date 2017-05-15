#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"
#include "astnode.h"
#include "errors.h"

class Parser {
    public:
        Parser(LexerTarget* _lexer);
        ~Parser();
        void setLexer(LexerTarget* _lexer);
        AstNode* parse();
    private:
        LexerTarget* mlexer;
};

#endif

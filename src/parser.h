#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"

enum class ParseErrorType {
    BadTopLevelStatement,
    BadImportName,
    MissImportSemicolon,
    MissPrototypeFn,
    BadPrototypeName,
    MissPrototypeLParen,
    BadFunctionParameter,
    MissOptparamColon,
    BadTypeIdentifier,
    BadOptparamTail,
    MissPrototypeColon,
    MissPrototypeSemicolon,
    BadVarName,
    MissVardecColon,
    BadAssign
};

class Parser {
    public:
        Parser(LexerTarget* _lexer);
        ~Parser();
        void setLexer(LexerTarget* _lexer);
        void parse();
    private:
        LexerTarget* mlexer;
};

#endif

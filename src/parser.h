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
    MissPrototypeRParen,
    BadBlockStart,
    IncompleteBlock,
    MissIfLParen,
    MissIfRParen,
    MissLParenFor,
    MissSemicolonFor1,
    MissSemicolonFor2,
    MissRParenFor,
    MissLParenWhile,
    MissRParenWhile,
    MissEqVarDecAssign,
    MissSemiReturn,
    BadAssign,
    Unknown
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

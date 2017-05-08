#ifndef ERRORS_H
#define ERRORS_H

#include <iostream>
#include <string>
#include "astnode.h"
#include "lexer.h"
#include "symboltable.h"

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
    MissForeign,
    MultipleScope,
    MissLParen,
    MissRParen,
    Unknown
};

enum class SemanticErrorType{
    MissmatchAssign,
    MissmatchVarDecAssign,
    MissmatchBinop,
    MissmatchFunctionParams,
    MissmatchReturnType,
    DerefNonPointer,
    DupDecl,
    UndefUse,
    NoFunc,
    OutLoop,
    NotLValue,
    DotOpLhs,
    DupFuncDef,
    MultipleFuncResolve,
    NoResolve,
    NoFunction,
    Unknown
};

void printErrorContext(int line, int col, std::string filename);
void semanticError(SemanticErrorType err, AstNode* n, SymbolTable* s);
int parse_error(ParseErrorType type, Token& t, LexerTarget* l);
#endif //ERRORS

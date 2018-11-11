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
    MissPrototypeFn,
    BadPrototypeName,
    BadFunctionParameter,
    BadTypeIdentifier,
    BadOptparamTail,
    BadVarName,
    BadBlockStart,
    IncompleteBlock,
    MissEqVarDecAssign,
    BadAssign,
    MissForeign,
    MultipleScope,
    MissLParen,
    MissRParen,
    MissSemicolon,
    MissColon,
    BadTypeModifier,
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
    CyclicTypeDefinitions,
    VoidPtrDeref,
    StructNotDefined,
    NegationPtr,
    TypeSizeAlignCalc,
    Unknown
};

void printErrorContext(int line, int col, const std::string& filename);
void semanticError(const SemanticErrorType err, AstNode* n, const SymbolTable* s);
void semanticError(const SemanticErrorType err, const std::vector<StructDefNode*>& types);
void semanticError(const SemanticErrorType err, const TypeInfo& type);
int parse_error(const ParseErrorType type, const Token& t, const LexerTarget* l);
#endif //ERRORS

#ifndef ASTTRANSFORMS_H
#define ASTTRANSFORMS_H
#include "astnode.h"

void collapseExpressionChains(AstNode* ast);
void checkContinueBreak(AstNode* ast, int loopDepth);
void fixOperatorAssociativity(AstNode* ast);
void decorateAst(AstNode* ast);
void typeCheckPass(AstNode* ast);
void populateSymbolTableFunctions(AstNode* ast);
void variableUseAndTypeCheck(AstNode* ast);
void variableUseCheck(AstNode* ast);
void printSymbolTable();

enum class SemanticErrorType{
    MissmatchAssign,
    MissmatchVarDecAssign,
    MissmatchBinop,
    MissmatchFunctionParams,
    MissmatchReturnType,
    Unknown
};

#endif

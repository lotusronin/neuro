#ifndef ASTTRANSFORMS_H
#define ASTTRANSFORMS_H
#include "astnode.h"
#include "symboltable.h"

void collapseExpressionChains(AstNode* ast);
void checkContinueBreak(AstNode* ast, int loopDepth);
void fixOperatorAssociativity(AstNode* ast);
void decorateAst(AstNode* ast);
void typeCheckPass(AstNode* ast);
void populateSymbolTableFunctions(AstNode* ast);
void populateTypeList(AstNode* ast);
void variableUseAndTypeCheck(AstNode* ast);
void variableUseCheck(AstNode* ast);
void printSymbolTable();
void deferPass(AstNode* ast);
void resolveSizeOfs(AstNode* ast);
void checkAssignments(AstNode* ast);
SymbolTable* getSymtab(std::string& file);

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
    Unknown
};

#endif

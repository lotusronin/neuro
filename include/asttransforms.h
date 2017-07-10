#ifndef ASTTRANSFORMS_H
#define ASTTRANSFORMS_H
#include "astnode.h"
#include "symboltable.h"
#include "errors.h"

void checkContinueBreak(AstNode* ast, int loopDepth);
void decorateAst(AstNode* ast);
void typeCheckPass(AstNode* ast);
void populateSymbolTableFunctions(AstNode* ast);
void populateTypeList(AstNode* ast);
void variableUseAndTypeCheck(AstNode* ast);
void variableUseCheck(AstNode* ast);
void printSymbolTable();
void deferPass(AstNode* ast);
void resolveSizeOfs(AstNode* ast);
void checkAssignments(AstNode* ast, SymbolTable* symTab);
void checkAssignments(AstNode* ast);
void importPrepass(AstNode* root);
void transformAssignments(AstNode* ast);
SymbolTable* getSymtab(std::string& file);

#endif

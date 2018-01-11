#ifndef ASTTRANSFORMS_H
#define ASTTRANSFORMS_H
#include "astnode.h"
#include "symboltable.h"
#include "errors.h"

void typeCheckPass(AstNode* ast);
void populateSymbolTableFunctions(AstNode* ast);
void populateTypeList(AstNode* ast);
void printSymbolTable();
void deferPass(AstNode* ast);
void resolveSizeOfs(AstNode* ast);
void semanticPass1(AstNode* ast);
void importPrepass(AstNode* root);
void transformAssignments(AstNode* ast);
SymbolTable* getSymtab(std::string& file);

#endif

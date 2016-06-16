#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <vector>
#include <unordered_map>
#include <utility>
#include <string>
#include "astnode.h"


struct SymbolTableEntry {
    AstNode* node;
    SemanticType type;
    std::vector<std::pair<SemanticType,AstNode*>> funcParams;
};

struct SymbolTable {
    std::unordered_map<std::string,SymbolTableEntry*> table;
    std::unordered_map<std::string,SymbolTable*> children;
    SymbolTable* parent = nullptr;
    unsigned int scope = 0;
};


SymbolTable* addNewScope(SymbolTable* s, std::string name);
void addVarEntry(SymbolTable* s, SemanticType t, AstNode* n);
void addFuncEntry(SymbolTable* s, SemanticType t, AstNode* n, const std::vector<std::pair<SemanticType,AstNode*>>& p);
std::vector<SymbolTableEntry*> getEntry(SymbolTable* s, const std::string& name);
void printTable(SymbolTable* s);
#endif

#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <vector>
#include <unordered_map>
#include <utility>
#include <string>
#include "astnode.h"
#include "astnodetypes.h"


struct SymbolTableEntry {
    AstNode* node;
    SemanticType type;
    TypeInfo typeinfo;
    std::vector<AstNode*> overloads; 
    std::vector<std::pair<TypeInfo,AstNode*>> funcParams;
    std::vector<std::pair<TypeInfo,AstNode*>> funcParamsTypeInfo;
    void* address;
};

struct SymbolTable {
    std::unordered_map<std::string,SymbolTableEntry*> table;
    std::unordered_map<std::string,SymbolTable*> children;
    std::unordered_map<std::string,SymbolTable*> imports;
    SymbolTable* parent = nullptr;
    std::string name = "global";
    unsigned int scope = 0;
};


SymbolTable* addNewScope(SymbolTable* s, std::string name);
SymbolTable* getScope(SymbolTable* s, std::string name);
SymbolTable* getFileScope(SymbolTable* s);

void addVarEntry(SymbolTable* s, const TypeInfo& t, std::string name);
void addVarEntry(SymbolTable* s, SemanticType t, AstNode* n);
void updateVarEntry(SymbolTable* s, SemanticType t, const std::string& name);
void updateVarEntry(SymbolTable* s, const TypeInfo& t, const std::string& name);
void addFuncEntry(SymbolTable* s, SemanticType t, AstNode* n, const std::vector<std::pair<TypeInfo,AstNode*>>& p);
void addFuncEntry(SymbolTable* s, FuncDefNode* n);
std::vector<SymbolTableEntry*> getEntry(SymbolTable* s, const std::string& name);
std::vector<SymbolTableEntry*> getEntry(SymbolTable* s, const std::string& name, const std::string& scope);
SymbolTableEntry* getFirstEntry(SymbolTable* s, const std::string& name);
SymbolTableEntry* getEntryCurrentScope(SymbolTable*s, const std::string& name);
const std::vector<SymbolTableEntry*> getFunctionEntries(SymbolTable* s);
void printTable(SymbolTable* s);
#endif

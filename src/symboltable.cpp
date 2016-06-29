#include "symboltable.h"
#include "astnodetypes.h"
#include <iostream>
#include <utility>

SymbolTable* addNewScope(SymbolTable* s, std::string name) {
    if(s->children.find(name) == s->children.end()) {
        auto scope = new SymbolTable;
        s->children.insert(std::make_pair(name, scope));
        scope->parent = s;
        scope->scope = s->scope+1;
        return scope;
    } else {
        return s->children.find(name)->second;
    }
}

void addVarEntry(SymbolTable* s, SemanticType t, AstNode* n) {
    auto entry = new SymbolTableEntry;
    entry->node = n;
    entry->type = t;
    std::string name = ((VarNode*)n)->getVarName();
    s->table.insert(std::make_pair(name,entry));
}

void addFuncEntry(SymbolTable* s, SemanticType t, AstNode* n, const std::vector<std::pair<SemanticType,AstNode*>>& p) {
    auto entry = new SymbolTableEntry;
    entry->node = n;
    entry->type = t;
    entry->funcParams = p;
    std::string funcname;
    if(n->nodeType() == AstNodeType::Prototype) {
        funcname = ((PrototypeNode*)n)->mfuncname;
    } else {
        funcname = ((FuncDefNode*)n)->mfuncname;
    }
    s->table.insert(std::make_pair(funcname,entry));
}

std::vector<SymbolTableEntry*> getEntry(SymbolTable* s, const std::string& name) {
    /* Returns a vector of entries that match the given input string.
     * If the vector is empty you have a use before define error.
     * If you have more than one entry you have conflicting globally
     * imported symbols.
     */
    //TODO(marcus): should conflicting imported names be an error? Can just report it.
    std::vector<SymbolTableEntry*> ret;
    auto exists = s->table.find(name);
    if(exists == s->table.end()) {
        if(s->parent == nullptr) {
            for(auto c : s->children) {
                exists = s->table.find(name);
                if(exists != s->table.end()) {
                    ret.push_back(exists->second);
                }
            }
            return ret;
        } else {
            //go up the tree
            return getEntry(s->parent, name);
        }
    } else {
        //found it, add it
        ret.push_back(exists->second);
    }
    return ret;
}

void printTable(SymbolTable* s) {
    if(s == nullptr) {
        return;
    }
    std::cout << "SymbolTable (scope " << s->scope << ")\nentries:\n";
    for(auto& e : s->table) {
        std::cout << e.first << ": " << e.second->type << "\n";
    }
    std::cout << "children:\n";
    for(auto& c : s->children) {
        std::cout << c.first << " ";
    }
    std::cout << "\n\n";
    for(auto& c : s->children) {
        printTable(c.second);
    }
}
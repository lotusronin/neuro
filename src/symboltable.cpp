#include "symboltable.h"
#include "astnodetypes.h"
#include <iostream>
#include <utility>

SymbolTable* addNewScope(SymbolTable* s, std::string name) {
    if(s->children.find(name) == s->children.end()) {
        //std::cout << "Adding new scope for " << name << "\n";
        auto scope = new SymbolTable;
        s->children.insert(std::make_pair(name, scope));
        scope->parent = s;
        scope->scope = s->scope+1;
        scope->name = name;
        return scope;
    } else {
        return s->children.find(name)->second;
    }
}

SymbolTable* getScope(SymbolTable* s, std::string name) {
    //std::cout << "Getting scope for " << name << '\n';
    auto res = s->children.find(name);
    if(res == s->children.end()) {
        return addNewScope(s,name);
    } else {
        return res->second;
    }
}

void addVarEntry(SymbolTable* s, SemanticType t, AstNode* n) {
    auto entry = new SymbolTableEntry;
    entry->node = n;
    entry->type = t;
    std::string name = ((VarNode*)n)->getVarName();
    s->table.insert(std::make_pair(name,entry));
}

void addVarEntry(SymbolTable* s, TypeInfo t, std::string name) {
    auto entry = new SymbolTableEntry;
    entry->typeinfo = t;
    s->table.insert(std::make_pair(name,entry));
}

void updateVarEntry(SymbolTable* s, SemanticType t, const std::string& name) {
    //std::cout << "updating var entry!\n";
    auto entry = s->table.find(name);
    if(entry != s->table.end()) {
        entry->second->type = t;
    }
}

void updateVarEntry(SymbolTable* s, TypeInfo t, const std::string& name) {
    //std::cout << __FUNCTION__ << " : updating var\n";
    auto entry = s->table.find(name);
    if(entry != s->table.end()) {
        entry->second->type = t.type;
        entry->second->typeinfo = t;
    } else {
        //std::cout << "Var not updated!!! TO IMPLEMENT!!!!\n";
    }
}

void addFuncEntry(SymbolTable* s, FuncDefNode* n) {
    auto entry = getFirstEntry(s,n->mfuncname);
    if(!entry) {
        //make a new entry
        entry = new SymbolTableEntry;
        entry->node = n;
        entry->type = n->mtypeinfo.type;
        s->table.insert(std::make_pair(n->mfuncname,entry));
    }
    entry->overloads.push_back(n);
}

void addFuncEntry(SymbolTable* s, SemanticType t, AstNode* n, const std::vector<std::pair<SemanticType,AstNode*>>& p) {
    auto entry = new SymbolTableEntry;
    entry->node = n;
    entry->type = t;
    entry->typeinfo.type = t;
    entry->funcParams = p;
    std::string funcname;
    if(n->nodeType() == AstNodeType::Prototype) {
        funcname = ((PrototypeNode*)n)->mfuncname;
    } else {
        funcname = ((FuncDefNode*)n)->mfuncname;
        funcname += std::to_string(((FuncDefNode*)n)->id);
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
    //std::cout << "Getting Entry for " << name << '\n';
    //std::cout << "Looking in " << s->name << " (scope = " << s->scope << ")\n";
    if(!s)  {
        //std::cout << "DANGER WILL ROBINSON! SymbolTable is NULL!!\n";
        return ret;
    }
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

SymbolTableEntry* getEntryCurrentScope(SymbolTable*s, const std::string& name) {
    ////std::cout << "Looking up entry with name " << name << " in current scope " << s->name << "...\n";
    SymbolTableEntry* res = nullptr;
    auto exists = s->table.find(name);
    if(exists != s->table.end()) {
        res = exists->second;
    }
    //printTable(s);
    return res;
}

SymbolTableEntry* getFirstEntry(SymbolTable* s, const std::string& name) {
    SymbolTableEntry* res = nullptr;
    auto entries = getEntry(s,name);
    if(entries.size() > 0) {
        res = entries[0];
    }
    return res;

}

void printTable(SymbolTable* s) {
    if(s == nullptr) {
        return;
    }
    std::cout << "SymbolTable " << s->name << " (scope " << s->scope << ")\nentries:\n";
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

const std::vector<SymbolTableEntry*> getFunctionEntries(SymbolTable* s) {
    //TODO(marcus): Currently a hack, should actually look up exported functions
    //currently this only looks in global scope of one file 
    std::vector<SymbolTableEntry*> ret;
    for(auto& str_entry : s->table) {
        auto entry = str_entry.second;
        if(entry->node->nodeType() == AstNodeType::FuncDef) {
            ret.push_back(entry);
        }
    }

    return ret;
}

std::vector<SymbolTableEntry*> getEntry(SymbolTable* s, const std::string& name, const std::string& scope) {
    /* Returns a vector of entries that match the given input string.
     * If the vector is empty you have a use before define error.
     * If you have more than one entry you have conflicting globally
     * imported symbols.
     */
    std::vector<SymbolTableEntry*> ret;
    auto tmp_s = s;
    while(tmp_s->parent->name != "global") {
        tmp_s = tmp_s->parent;
    }
    auto visible = tmp_s->imports.find(scope+".nro");
    if(visible != tmp_s->imports.end()) {
        auto scopedToFile = visible->second;
        auto exists = scopedToFile->table.find(name);
        if(exists != scopedToFile->table.end()) {
            //std::cout << "Found " << name << " in scope " << scope << '\n';
            ret.push_back(exists->second);
        } else {
            std::cout << "Didn't find function " << name << '\n';
        }
    } else {
        std::cout << "Didn't find scope " << scope << '\n';
    }
    return ret;
}

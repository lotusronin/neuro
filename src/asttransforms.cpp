#include "asttransforms.h"
#include "astnodetypes.h"
#include "astnode.h"
#include "symboltable.h"
#include <iostream>
#include <utility>

#define ANT AstNodeType
static void typeCheckPass(AstNode* ast, SymbolTable* symTab);
static void populateSymbolTableFunctions(AstNode* ast, SymbolTable* symTab);
static void variableUseCheck(AstNode* ast, SymbolTable* symTab);

void collapseExpressionChains(AstNode* ast) {
    std::vector<AstNode*>* vec = ast->getChildren();
    for(unsigned int i = 0; i < vec->size(); i++) {
        AstNode* child = (*vec)[i];
        if(child->nodeType() == ANT::BinOp) {
            while(child->nodeType() == ANT::BinOp) {
                BinOpNode* node = (BinOpNode*)child;
                std::string op = node->getOp();
                if(op.compare("expression") == 0) {
                    if(node->mchildren.size() == 1) {
                        std::cout << "Deleting child!!!\n";
                        (*vec)[i] = node->mchildren[0];
                        delete node;
                        child = (*vec)[i];
                    }
                } else if(op.compare("multdivexpr") == 0) {
                    if(node->mchildren.size() == 1) {
                        std::cout << "Deleting child!!!\n";
                        (*vec)[i] = node->mchildren[0];
                        delete node;
                        child = (*vec)[i];
                    }
                } else if(op.compare("parenexpr") == 0) {
                    std::cout << "Deleting child!!!\n";
                    (*vec)[i] = node->mchildren[0];
                    delete node;
                    child = (*vec)[i];
                } else {
                    break;
                }
            }
        }
    }
    for(auto c : (*vec)) {
        collapseExpressionChains(c);
    }
}

void checkContinueBreak(AstNode* ast, int loopDepth) {
    AstNodeType type = ast->nodeType();
    if(type == AstNodeType::LoopStmt) {
        if(loopDepth == 0) {
            std::cout << "Error, Break or Continue used outside of a loop!\n";
        }
    }

    int nextLoopDepth = loopDepth;
    if(type == AstNodeType::ForLoop || type == AstNodeType::WhileLoop) {
        nextLoopDepth += 1;
    }
    for(auto c : (*(ast->getChildren()))) {
        checkContinueBreak(c, nextLoopDepth);
    }
}

void fixOperatorAssociativity(AstNode* ast) {
    std::vector<AstNode*>* children = ast->getChildren();
    for(unsigned int i = 0; i < children->size(); i++) {
        auto type = (*children)[i]->nodeType();
        if(type == ANT::BinOp) {
            auto child = (BinOpNode*)(*children)[i];
            //TODO(marcus): should replace this constant with an enum or macro
            if(child->getPriority() == 3) {
                //skip () nodes since they only have 1 child
                continue;
            }
            while(child->RHS()->nodeType() == ANT::BinOp) {
                auto newChild = (BinOpNode*)child->RHS();
                if(child->getPriority() == newChild->getPriority()) {
                    //Rotate tree to the left if it is the same operator
                    auto oldChildNewRHS = newChild->LHS();
                    newChild->setLHS(child);
                    child->setRHS(oldChildNewRHS);
                    (*children)[i] = newChild;
                    child = newChild;
                } else {
                    break;
                }
            }
        }
    }

    for(auto c : (*children)) {
        fixOperatorAssociativity(c);
    }
}

void decorateAst(AstNode* ast) {
    for(auto c : (*(ast->getChildren()))) {
        decorateAst(c);
        c->getType();
    }
}

//TODO(marcus): should this be global? is there a better way?
SymbolTable progSymTab;

void populateSymbolTableFunctions(AstNode* ast) {
    for(auto c : (*(ast->getChildren()))) {
        switch(c->nodeType()) {
            case ANT::CompileUnit:
                {
                auto scope = addNewScope(&progSymTab, ((CompileUnitNode*)c)->getFileName());
                populateSymbolTableFunctions(c, scope);
                }
                break;
            default:
                std::cout << "unknown ast node type!\n";
                break;
        }
    }
}

static void populateSymbolTableFunctions(AstNode* ast, SymbolTable* symTab) {
    for(auto c : (*(ast->getChildren()))) {
        std::vector<std::pair<SemanticType,AstNode*>> p;
        std::vector<AstNode*>* params;
        SemanticType retType;
        switch(c->nodeType()) {
            case ANT::Prototype:
                {
                    auto proto = (PrototypeNode*)c;
                    retType = proto->getType();
                    params = proto->getParameters();
                    for(auto prm : (*params)) {
                        SemanticType prmtyp = prm->getChildren()->at(0)->getType();
                        p.push_back(std::make_pair(prmtyp,prm));
                    }
                    addFuncEntry(symTab, retType, c, p);
                    delete params;
                }
                break;
            case ANT::FuncDef:
                {
                    auto func = (FuncDefNode*)c;
                    retType = func->getType();
                    params = func->getParameters();
                    for(auto prm : (*params)) {
                        SemanticType prmtyp = prm->getChildren()->at(0)->getType();
                        p.push_back(std::make_pair(prmtyp,prm));
                    }
                    addFuncEntry(symTab, retType, c, p);
                    delete params;
                }
                break;
            default:
                break;
        }
    }
}

void typeCheckPass(AstNode* ast) {

    for(auto c : (*(ast->getChildren()))) {
        switch(c->nodeType()) {
            case ANT::CompileUnit:
                {
                auto scope = addNewScope(&progSymTab, ((CompileUnitNode*)ast)->getFileName());
                typeCheckPass(c, scope);
                }
                break;
            default:
                std::cout << "unknown ast node type!\n";
                break;
        }
    }

}

static void typeCheckPass(AstNode* ast, SymbolTable* symTab) {
    for(auto c : (*(ast->getChildren()))) {
        switch(c->nodeType()) {
            case ANT::Prototype:
                std::cout << "ast node is prototype, adding to symbol table\n";
                break;
            case ANT::FuncDef:
                std::cout << "ast node is function, adding to symbol table\n";
                break;
            default:
                std::cout << "unknown ast node type!\n";
                break;
        }
    }

}

void variableUseAndTypeCheck(AstNode* ast) {
    variableUseCheck(ast);
}

void variableUseCheck(AstNode* ast) {
    std::cout << "beginning variable use check!\n";
    for(auto c : (*(ast->getChildren()))) {
        variableUseCheck(c,&progSymTab);
    }
}

static void variableUseCheck(AstNode* ast, SymbolTable* symTab) {
    for(auto c : (*(ast->getChildren()))) {
        switch(c->nodeType()) {
            case ANT::FuncDef:
                {
                    std::string name = ((FuncDefNode*)c)->mfuncname;
                    auto scope = addNewScope(symTab, name);
                    variableUseCheck(c,scope);
                }
                break;
            case ANT::Block:
                {
                    auto scope = addNewScope(symTab, std::to_string(((BlockNode*)c)->getId()));
                    variableUseCheck(c,scope);
                }
                break;
            case ANT::IfStmt:
                break;
            case ANT::ForLoop:
            case ANT::WhileLoop:
                break;
            case ANT::Assign:
                break;
            case ANT::VarDec:
                {
                    std::cout << "Var Dec!!!\n";
                    std::string name = ((VarNode*)(c->getChildren()->at(0)))->getVarName();
                    auto entry = getEntry(symTab,name);
                    if(entry.size() != 0) {
                        std::cout << "Error, you declared var " << name << " previously!\n";
                    } else {
                        //std::cout << "Adding variable declaration entry!\n";
                        addVarEntry(symTab, SemanticType::Typeless, c->getChildren()->at(0));
                    }
                }
                break;
            case ANT::VarDecAssign:
                {
                    std::cout << "Var Dec Assign!!!\n";
                    std::string name = ((VarNode*)(c->getChildren()->at(0)))->getVarName();
                    auto entry = getEntry(symTab,name);
                    if(entry.size() != 0) {
                        std::cout << "Error, you declared var " << name << " previously!\n";
                    } else {
                        //std::cout << "Adding variable declaration entry!\n";
                        addVarEntry(symTab, SemanticType::Typeless, c->getChildren()->at(0));
                    }
                    //TODO(marcus): check expression
                }
                break;
            case ANT::Expression:
                break;
            case ANT::Var:
                {
                    std::string name = ((VarNode*)(c))->getVarName();
                    auto entry = getEntry(symTab,name);
                    if(entry.size() == 0) {
                        std::cout << "Variable " << name << " was used before it was defined!\n";
                    }
                }
                break;
            case ANT::FuncCall:
                {
                    std::string name = ((FuncCallNode*)c)->mfuncname;
                    auto entry = getEntry(symTab,name);
                    if(entry.size() == 0) {
                        std::cout << "Function " << name << " called before it was defined.\n";
                    }
                    //TODO(marcus): check function params
                }
                break;
            default:
                break;
        }
    }
}

void printSymbolTable() {
    printTable(&progSymTab);
}
/*enum class AstNodeType {
    Program,
    CompileUnit,
    TLStmnt,
    ImportStmnt,
    Prototype,
    Params,
    Type,
    Var,
    VarDec,
    VarDecAssign,
    FuncDef,
    Block,
    Stmt,
    IfStmt,
    ElseStmt,
    ForLoop,
    DeferStmt,
    WhileLoop,
    RetStmnt,
    Expression,
    BinOp,
    FuncCall,
    Const,
    Args,
    LoopStmt,
    Assign,
    Default
};*/

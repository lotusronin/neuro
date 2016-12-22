#include "asttransforms.h"
#include "astnodetypes.h"
#include "astnode.h"
#include "symboltable.h"
#include "coloroutput.h"
#include <iostream>
#include <string>
#include <utility>

extern bool semantic_error;

#define ANT AstNodeType
static void typeCheckPass(AstNode* ast, SymbolTable* symTab);
static void populateSymbolTableFunctions(AstNode* ast, SymbolTable* symTab);
static void variableUseCheck(AstNode* ast, SymbolTable* symTab);
static SemanticType getType(AstNode* ast, SymbolTable* symTab);
static void typeCheckExpression(AstNode* ast, SymbolTable* symTab);

static void semanticError(SemanticErrorType err, SemanticType lt, SemanticType rt) {
    std::cout << "Semantic Error!!!\n";
    switch(err) {
    case SemanticErrorType::MissmatchAssign:
        {
            ERROR("Assign Type Error: Cannot assign " << rt << " to a variable of type " << lt << '\n');
        }
        break;
    case SemanticErrorType::MissmatchVarDecAssign:
        {
            ERROR("Variable Decl and Assign Type Error: Cannot assign " << rt << " to a variable of type " << lt << '\n');
        }
        break;
    case SemanticErrorType::MissmatchBinop:
        {
            ERROR("Binary Operator had conflicting types: " << lt << " binop " << rt << '\n');
        }
        break;
    case SemanticErrorType::MissmatchFunctionParams:
        {
            ERROR("Function parameter expected " << rt << " but was given " << lt << '\n');
        }
        break;
    case SemanticErrorType::MissmatchReturnType:
        {
            ERROR("Return value of type " << lt << " does not match function type " << rt << '\n');
        }
        break;
    case SemanticErrorType::Unknown:
        {
        }
        break;
    default:
        //Let's hope we never get here or something is terribly wrong... :)
        std::cout << "UNKNOWN SEMANTIC ERROR!\n";
        break;
    }
    semantic_error = true;
    return;
}

void collapseExpressionChains(AstNode* ast) {
    //TODO(marcus): clean this up.
    std::vector<AstNode*>* vec = ast->getChildren();
    for(unsigned int i = 0; i < vec->size(); i++) {
        AstNode* child = (*vec)[i];
        if(child->nodeType() == ANT::BinOp) {
            while(child->nodeType() == ANT::BinOp) {
                BinOpNode* node = (BinOpNode*)child;
                std::string op = node->getOp();
                //string op of largest binop node that is meaningful is '( )' with a length of 3
                //so anything with a length over that is just a node in the chain
                bool nonop = (op.size() > 3);
                //bool nonop = (op.compare("expression") == 0) || (op.compare("multdivexpr") == 0) || (op.compare("parenexpr") == 0) || (op.compare("plusminexpr") == 0) || (op.compare("gtelteexpr") == 0);
                if(nonop) {
                    if(node->mchildren.size() == 1) {
                        std::cout << "Deleting child!!!\n";
                        (*vec)[i] = node->mchildren[0];
                        delete node;
                        child = (*vec)[i];
                    }
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

//TODO(marcus): Currently this is global. Is there a better way to do this? Maybed a list of types
//per file?
//a list of user defined types with a mapping of member name to member index
std::unordered_map<std::string,std::unordered_map<std::string,int>*> userTypesList;

static void registerTypeDef(StructDefNode* n) {
    auto type_name = n->ident;
    if(userTypesList.find(type_name) != userTypesList.end()) {
        //TODO(marcus): We already have the type registered
        //should we error?
        return;
    }
    std::unordered_map<std::string,int>* member_map = new std::unordered_map<std::string,int>();
    int index = 0;
    for(auto c : n->mchildren) {
        //TODO(marcus): Might want to check that we have a vardec node if we
        //allow other things in function body definitions.
        auto vdec = (VarDecNode*) c;
        auto v = (VarNode*) vdec->getLHS();
        std::string member_name = v->getVarName();
        member_map->insert(std::make_pair(member_name,index));
        ++index;
    }

    userTypesList.insert(std::make_pair(type_name,member_map));
}

void populateTypeList(AstNode* ast) {
    //We only need to go through the list of top level statements
    //of each program as we won't ever have struct definitions occur in
    //function definitions, prototypes, etc, so we can skip any of these 
    //kinds of nodes.
    for(auto c : (*(ast->getChildren()))) {
        switch(c->nodeType()) {
            case ANT::CompileUnit:
                populateTypeList(c);
                break;
            case ANT::StructDef:
                //parse definition!
                registerTypeDef((StructDefNode*)c);
                break;
            default:
                continue;
                break;
        }
    }
}

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

SymbolTable* getSymtab(std::string& file) {
    return progSymTab.children.find(file)->second;
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
    std::cout << "Type Checking Pass\n";
    for(auto c : (*(ast->getChildren()))) {
        switch(c->nodeType()) {
            case ANT::CompileUnit:
                {
                auto scope = addNewScope(&progSymTab, ((CompileUnitNode*)c)->getFileName());
                typeCheckPass(c, scope);
                }
                break;
            default:
                std::cout << "unknown ast node type!\n";
                break;
        }
    }

}

AstNode* currentFunc = nullptr;
static void typeCheckPass(AstNode* ast, SymbolTable* symTab) {
    for(auto c : (*(ast->getChildren()))) {
        switch(c->nodeType()) {
            case ANT::Prototype:
                std::cout << "ast node is prototype, adding to symbol table\n";
                break;
            case ANT::FuncDef:
                {
                    std::cout << "ast node is function, adding to symbol table\n";
                    auto scope = addNewScope(symTab, ((FuncDefNode*)c)->mfuncname);
                    currentFunc = c;
                    typeCheckPass(c,scope);
                    currentFunc = nullptr;
                }
                break;
            case ANT::Assign:
                {
                    std::cout << "Assign type check...\n";
                    auto assignnode = (AssignNode*)c;
                    auto lhs = (VarNode*)assignnode->getLHS();
                    auto rhs = assignnode->getRHS();

                    SymbolTableEntry* e = getEntryCurrentScope(symTab,lhs->getVarName());
                    if(e == nullptr) { std::cout << "SymbolTableEntry isn't valid\n"; break;}
                    SemanticType lt = e->type;
                    typeCheckExpression(rhs,symTab);
                    SemanticType rt = getType(rhs, symTab);

                    if(lt != rt) {
                        semanticError(SemanticErrorType::MissmatchAssign,lt,rt);
                    } else {
                        std::cout << "Assign Types matched: l,r " << lt << ", " << rt << "\n"; 
                    }

                }
                break;
            case ANT::VarDec:
                {
                    auto vdecn = (VarDecNode*)c;
                    auto lhs = (VarNode*)vdecn->getLHS();
                    auto rhs = vdecn->getRHS(); //type node
                    SemanticType st = rhs->getType();
                    lhs->mstype = st;
                    updateVarEntry(symTab,st,lhs->getVarName());
                }
                break;
            case ANT::VarDecAssign:
                {
                    std::cout << "VarDecAssign type check...\n";
                    auto vdan = (VarDecAssignNode*)c;
                    auto lhs = (VarNode*) vdan->getLHS();
                    auto rhs = vdan->getRHS();
                    SemanticType ltype;
                    typeCheckExpression(rhs,symTab);
                    SemanticType rtype = getType(rhs,symTab);
                    
                    if(lhs->mchildren.size() == 0) {
                        //Inferring type
                        std::cout << "inferring type!\n";
                        ltype = rtype;
                    } else {
                        ltype = lhs->mchildren.at(0)->getType();
                    }
                    updateVarEntry(symTab,ltype,lhs->getVarName());
                    if(ltype != rtype) {
                        semanticError(SemanticErrorType::MissmatchVarDecAssign,ltype,rtype);
                    } else {
                        std::cout << "VarDecAssign Types matched: l,r " << ltype << ", " << rtype << "\n"; 
                    }

                }
                break;
            case ANT::Block:
                {
                    auto scope = addNewScope(symTab, "block"+std::to_string(((BlockNode*)c)->getId()));
                    std::cout << "Entering into scope " << scope->name << "\n";
                    typeCheckPass(c,scope);
                }
            case ANT::FuncCall:
                typeCheckExpression(c,symTab);
                break;
            case ANT::RetStmnt:
                {
                    auto retn = (ReturnNode*)c;
                    auto expr = retn->mchildren.at(0);
                    typeCheckExpression(expr,symTab);
                    SemanticType rett = getType(expr,symTab);
                    SemanticType funcrett = currentFunc->getType();
                    if(rett != funcrett) {
                        semanticError(SemanticErrorType::MissmatchReturnType,rett,funcrett);
                    } else {
                        std::cout << "return type and function type matched!\n";
                    }
                }
                break;
            default:
                std::cout << "unknown ast node type!\n";
                break;
        }
    }

}

static void typeCheckExpression(AstNode* ast, SymbolTable* symTab) {
    AstNodeType nt = ast->nodeType();
    switch(nt) {
        case ANT::BinOp:
            {
                std::cout << "Type checking BinOp\n";
                auto node = (BinOpNode*)ast;
                if(node->getPriority() == 3) {
                    //if node is (), just check child
                    typeCheckExpression(node->LHS(),symTab);
                    return;
                }

                auto lhs = node->LHS();
                auto rhs = node->RHS();
                typeCheckExpression(lhs,symTab);
                auto lt = getType(lhs,symTab);
                typeCheckExpression(rhs,symTab);
                auto rt = getType(rhs,symTab);

                if(lt != rt) {
                    semanticError(SemanticErrorType::MissmatchBinop,lt,rt);
                } else {
                    std::cout << "Binary Op Types matched: l,r " << lt << ", " << rt << "\n"; 
                }
            }
            break;
        case ANT::FuncCall:
            {
                auto funccall = (FuncCallNode*)ast;
                auto entries = getEntry(symTab,funccall->mfuncname);
                std::cout << "Type checking function call " << funccall->mfuncname << "\n";
                if(entries.size() == 0) {
                    std::cout << "Function has no entry in the symbol table!\n";
                } else {
                    SymbolTableEntry* e = entries.at(0);
                    auto funcparams = e->funcParams;
                    if(funcparams.size() != funccall->margs.size()) {
                        std::cout << "Different number of arguments and function parameters!\n";
                    } else {
                        for(unsigned int i = 0; i < funcparams.size(); i++) {
                            auto n = funccall->margs.at(i);
                            typeCheckExpression(n,symTab);
                            SemanticType type1 = getType(n,symTab);
                            SemanticType type2 = funcparams.at(i).first;
                            if(type1 != type2) {
                                semanticError(SemanticErrorType::MissmatchFunctionParams,type1,type2);
                            } else {
                                std::cout << "Function parameter matched!\n";
                            }
                        }
                    }
                }
            }
            break;
        default:
            break;
    }
}

static SemanticType getType(AstNode* ast, SymbolTable* symTab) {
    AstNodeType nt = ast->nodeType();
    SemanticType res = SemanticType::Typeless;
    switch(nt) {
        case ANT::BinOp:
            {
                auto binopn = (BinOpNode*)ast;
                if(binopn->getPriority() == 3) {
                    //we have () node, just get child.
                    res = getType(binopn->LHS(),symTab);
                } else {
                    SemanticType lt = getType(binopn->LHS(),symTab);
                    SemanticType rt = getType(binopn->RHS(),symTab);
                    //TODO(marcus): support implicit casting!
                    //TODO(marcus): check priority of types!
                    res = lt;
                }
            }
            break;
        case ANT::Const:
            {
                res = ast->getType();
            }
            break;
        case ANT::FuncCall:
            {
                std::string name = ((FuncCallNode*)ast)->mfuncname;
                auto entries = getEntry(symTab,name);
                if(entries.size() == 0) {
                    std::cout << "Function " << name << " Not Found!!!\n";
                } else if(entries.size() > 1) {
                    std::cout << "Function " << name << " Had Multiple Matches!!!\n";
                } else {
                    SymbolTableEntry* e = entries.at(0);
                    res = e->type;
                }
            }
            break;
        case ANT::Var:
            {
                std::string name = ((VarNode*)ast)->getVarName();
                //TODO(marcus): Traverse Symbol tables but only get first matching entry!
                auto entries = getEntry(symTab,name);
                if(entries.size() == 0) {
                    std::cout << "Var " << name << " Not Found!!!\n";
                } else {
                    SymbolTableEntry* e = entries.at(0);
                    res = e->type;
                }
            }
            break;
        default:
            break;
    }
    return res;
}

void variableUseAndTypeCheck(AstNode* ast) {
    variableUseCheck(ast);
}

void variableUseCheck(AstNode* ast) {
    std::cout << "beginning variable use check!\n";
    for(auto c : (*(ast->getChildren()))) {
        auto sc = progSymTab.children.at(((CompileUnitNode*)c)->getFileName());
        variableUseCheck(c,sc);
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
                    auto scope = addNewScope(symTab, "block"+std::to_string(((BlockNode*)c)->getId()));
                    std::cout << "Entering into scope " << scope->name << "\n";
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
                    std::cout << "SymbolTable " << symTab->name << "\n";
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
                    std::cout << "Looking in symboltable for function " << name << "\n"; 
                    auto entry = getEntry(symTab,name);
                    if(entry.size() == 0) {
                        std::cout << "Function " << name << " called before it was defined.\n";
                    } else {
                        std::cout << "Function found\n";
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

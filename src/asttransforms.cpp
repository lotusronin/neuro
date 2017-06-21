#include "asttransforms.h"
#include "astnodetypes.h"
#include "astnode.h"
#include "symboltable.h"
#include "coloroutput.h"
#include <iostream>
#include <string>
#include <utility>

extern bool semantic_error;

//TODO(marcus): should this be global? is there a better way?
SymbolTable progSymTab;

#define ANT AstNodeType
#define SET SemanticErrorType
static void typeCheckPass(AstNode* ast, SymbolTable* symTab);
static void populateSymbolTableFunctions(AstNode* ast, SymbolTable* symTab);
static void variableUseCheck(AstNode* ast, SymbolTable* symTab);
TypeInfo getTypeInfo(AstNode* ast, SymbolTable* symTab);
static bool isSameType(TypeInfo& t1, TypeInfo& t2);

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

void checkContinueBreak(AstNode* ast, int loopDepth, SymbolTable* symTab) {
    AstNodeType type = ast->nodeType();
    if(type == AstNodeType::LoopStmt) {
        if(loopDepth == 0) {
            semanticError(SET::OutLoop, ast, symTab);
        }
    }

    int nextLoopDepth = loopDepth;
    if(type == AstNodeType::ForLoop || type == AstNodeType::WhileLoop) {
        nextLoopDepth += 1;
    }
    auto scope = symTab;
    switch(type) {
        case ANT::CompileUnit:
            scope = getScope(symTab, ((CompileUnitNode*)ast)->getFileName());
            break;
        case ANT::FuncDef:
            scope = getScope(symTab, ((FuncDefNode*)ast)->mfuncname + std::to_string(((FuncDefNode*)ast)->id));
            break;
        case ANT::Block:
            scope = getScope(symTab, "block"+std::to_string(((BlockNode*)ast)->getId()));
            break;
        case ANT::ForLoop:
            scope = getScope(symTab, "for"+std::to_string(((ForLoopNode*)ast)->getId()));
            break;
        case ANT::WhileLoop:
            scope = getScope(symTab, "while"+std::to_string(((WhileLoopNode*)ast)->getId()));
            break;
        default:
            break;
    }
    for(auto c : (*(ast->getChildren()))) {
        checkContinueBreak(c, nextLoopDepth, scope);
    }
}

void checkContinueBreak(AstNode* ast, int loopDepth) {
    checkContinueBreak(ast,loopDepth,&progSymTab);
}

void fixOperatorAssociativity(AstNode* ast) {
    std::vector<AstNode*>* children = ast->getChildren();
    for(unsigned int i = 0; i < children->size(); i++) {
        auto type = (*children)[i]->nodeType();
        if(type == ANT::BinOp) {
            auto child = (BinOpNode*)(*children)[i];
            //TODO(marcus): should replace this constant with an enum or macro
            if(child->getPriority() == 3 || child->getPriority() == 5) {
                //skip () nodes since they only have 1 child
                //skip &/@ addr-of and deref since they only have 1 child
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

//TODO(marcus): Currently this is global. Is there a better way to do this? Maybed a list of types
//per file?
//a list of user defined types with a mapping of member name to member index
std::unordered_map<std::string,std::unordered_map<std::string,int>*> userTypesList;
std::unordered_map<std::string,AstNode*> structList;

static void registerTypeDef(StructDefNode* n) {
    auto type_name = n->ident;
    if(userTypesList.find(type_name) != userTypesList.end()) {
        //TODO(marcus): We already have the type registered
        //should we error?
        return;
    }
    structList.insert(std::make_pair(type_name,n));
    std::unordered_map<std::string,int>* member_map = new std::unordered_map<std::string,int>();
    int index = 0;
    for(auto c : n->mchildren) {
        if(c->nodeType() != ANT::VarDec) {
            //TODO(marcus): report error unless we allow other things in struct body
            //definitions.
            continue;
        }
        auto vdec = (VarDecNode*) c;
        auto v = (VarNode*) vdec->getLHS();
        std::string member_name = v->getVarName();
        member_map->insert(std::make_pair(member_name,index));
        ++index;
    }

    userTypesList.insert(std::make_pair(type_name,member_map));
}
void populateTypeList(AstNode* ast, SymbolTable* sym) {
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
void populateTypeList(AstNode* ast) {
    for(auto c : (*(ast->getChildren()))) {
        switch(c->nodeType()) {
            case ANT::CompileUnit:
                {
                    auto scope = addNewScope(&progSymTab, ((CompileUnitNode*)c)->getFileName());
                    populateTypeList(c, scope);
                }
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
                //std::cout << "unknown ast node type!\n";
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
                        auto prmti = prm->mtypeinfo;
                        SemanticType prmtyp = prmti.type;
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
                    auto entries = getEntry(symTab,func->mfuncname);
                    if(entries.size() != 0) {
                        auto e = entries.at(0);
                        auto numParams = params->size();
                        //For every overload
                        for(auto ol : e->overloads) {
                            if(numParams == ol->mchildren.size()-1) {
                                bool repeat = true;
                                //check for any mismatched param types
                                for(unsigned int i = 0; i < numParams; ++i) {
                                    if(!isSameType(params->at(i)->mtypeinfo,ol->mchildren.at(i)->mtypeinfo)) {
                                        repeat = false;
                                        break;
                                    }
                                }
                                if(repeat) {
                                    semanticError(SemanticErrorType::DupFuncDef, ol, symTab);
                                }
                            }
                        }
                    }
                    for(auto prm : (*params)) {
                        auto prmti = prm->mtypeinfo;
                        SemanticType prmtyp = prmti.type;
                        p.push_back(std::make_pair(prmtyp,prm));
                    }
                    addFuncEntry(symTab, retType, c, p);
                    addFuncEntry(symTab, func);
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
            //case ANT::StructDef:
                //break;
            default:
                //std::cout << "unknown ast node type!\n";
                break;
        }
    }

}

static bool isSameType(TypeInfo& t1, TypeInfo& t2) {
    if(t1.indirection == t2.indirection) {
        if(t1.type == t2.type) {
            if(t1.type != SemanticType::User) {
                return true;
            } else {
                return (t1.userid.compare(t2.userid) == 0);
            }
        }
    }
    return false;
}

#define ST SemanticType
bool isPointerMath(TypeInfo& t1, TypeInfo& t2) {
    if(t1.indirection > 0 && t2.indirection > 0) {
        return false;
    }

    SemanticType st;
    if(t1.indirection > 0) {
        st = t2.type;
    } else if(t2.indirection > 0) {
        st = t1.type;
    }

    switch(st) {
        case ST::u8:
        case ST::u16:
        case ST::u32:
        case ST::u64:
        case ST::s8:
        case ST::s16:
        case ST::s32:
        case ST::s64:
        case ST::Int:
        case ST::intlit:
            return true;
            break;
        default:
            break;
    }

    return false;
}

static bool canCast(TypeInfo& t1, TypeInfo& t2) {
    //helper function to check if we can implicitly cast t2 to the t1

    if(t1.type == SemanticType::User && t2.type == SemanticType::User) {
        return (t1.userid.compare(t2.userid) == 0);
    }

    if(t1.indirection != t2.indirection) {
        //can't implicitly convert two diff pointer types
        return false;
    }

    if(t1.indirection == 1 && t2.indirection == 1) {
        if(t1.type == SemanticType::u8 || t1.type == SemanticType::Char) {
            if(t2.type == SemanticType::u8 || t2.type == SemanticType::Char) {
                return true;
            }
        }
    }

    if(t1.indirection > 0 || t2.indirection > 0) {
        //can't implicitly convert two diff pointer types
        //TODO(marcus): what about void*?
        return false;
    }

    //TODO(marcus): review implicit casting logic
    // u8 -> u32 okay
    // u8 -> u64 okay
    // u32 -> u64 okay
    // s8 -> s32 okay
    // s8 -> s64 okay
    // s32 -> s64 okay
    // f32 -> f64 okay
    // u8,u32 -> f32 okay
    // u8,u32,u64 -> f64 okay
    // s8,s32 -> f32 okay
    // s8,s32,s64 -> f64 okay
    if(t1.type == ST::Double) {
        return true;
    }
    if(t1.type == ST::Float) {
        //TODO(marcus): check for (u/s)64 types
        return true;
    }
    if(t2.type == ST::intlit) {
        switch(t1.type) {
            case ST::u8:
            case ST::u32:
            case ST::u64:
            case ST::s8:
            case ST::s32:
            case ST::s64:
            case ST::Int:
                return true;
                break;
            default:
                return false;
                break;
        }
    }

    if(t2.type == ST::u8) {
        switch(t1.type) {
            case ST::u32:
            case ST::u64:
            case ST::s8:
            case ST::s32:
            case ST::s64:
            case ST::Char:
                return true;
                break;
            default:
                return false;
                break;
        }
    }

    if(t2.type == ST::u32) {
        switch(t1.type) {
            case ST::Int:
            case ST::u64:
            case ST::s32:
            case ST::s64:
                return true;
                break;
            default:
                return false;
                break;
        }
    }

    if(t2.type == ST::Int) {
        switch(t1.type) {
            case ST::u32:
            case ST::u64:
                return true;
                break;
            default:
                return false;
                break;
        }
    }

    if(t2.type == ST::s8) {
        switch(t1.type) {
            case ST::s32:
            case ST::s64:
                return true;
                break;
            default:
                return false;
                break;
        }
    }

    if(t2.type == ST::s32) {
        switch(t1.type) {
            case ST::s64:
                return true;
                break;
            default:
                return false;
                break;
        }
    }

    // *u8 = string literal
    if(t2.type == ST::Char) {
        if(t1.type == ST::u8) {
            if(t1.indirection == 1) {
                return true;
            }
        }
    }

    if(t2.type == ST::Char) {
        switch(t1.type) {
            case ST::intlit:
            case ST::u32:
            case ST::Int:
                return true;
                break;
            default:
                return false;
                break;
            }
    }

    return false;
}
#undef ST

static int decreaseDerefTypeInfo(TypeInfo& t) {
    if(t.indirection > 0) {
        t.indirection -= 1;
    } else {
        return 1;
    }
    return 0;
}

static void increaseDerefTypeInfo(TypeInfo& t) {
    t.indirection += 1;
    return;
}

bool resolveFunction(FuncCallNode* funccall, SymbolTable* symTab);

AstNode* currentFunc = nullptr;
std::string currentFunc2;
static void typeCheckPass(AstNode* ast, SymbolTable* symTab) {
    for(auto c : (*(ast->getChildren()))) {
        switch(c->nodeType()) {
            case ANT::FuncDef:
                {
                    //std::cout << __FILE__ << ':' << __FUNCTION__ << " FunctionDef!\n";
                    auto scope = getScope(symTab, ((FuncDefNode*)c)->mfuncname + std::to_string(((FuncDefNode*)c)->id));
                    currentFunc2 = ((FuncDefNode*)c)->mfuncname;
                    currentFunc = c;
                    typeCheckPass(c,scope);
                    currentFunc2 = "";
                    currentFunc = nullptr;
                    //std::cout << "done with function " << ((FuncDefNode*)c)->mfuncname << '\n';
                }
                break;
            case ANT::Block:
                {
                    //std::cout << __FILE__ << ':' << __FUNCTION__ << " Block!\n";
                    //std::cout << "Block ID " << ((BlockNode*)c)->getId() << '\n';
                    auto scope = getScope(symTab, "block"+std::to_string(((BlockNode*)c)->getId()));
                    typeCheckPass(c,scope);
                }
                break;
            case ANT::ForLoop:
                {
                    //std::cout << __FILE__ << ':' << __FUNCTION__ << " For!\n";
                    auto scope = getScope(symTab, "for"+std::to_string(((ForLoopNode*)c)->getId()));
                    typeCheckPass(c,scope);
                }
                break;
            case ANT::WhileLoop:
                {
                    //std::cout << __FILE__ << ':' << __FUNCTION__ << " While!\n";
                    auto scope = getScope(symTab, "while"+std::to_string(((WhileLoopNode*)c)->getId()));
                    typeCheckPass(c,scope);
                }
                break;
            case ANT::IfStmt:
                {
                    //std::cout << __FILE__ << ':' << __FUNCTION__ << " If!\n";
                    typeCheckPass(c,symTab);
                }
                break;
            case ANT::FuncCall:
                {
                    //std::cout << __FILE__ << ':' << __FUNCTION__ << " FuncCall!\n";
                    auto funccall = (FuncCallNode*)c;
                    std::string funcname = funccall->mfuncname;
                    auto entries = getEntry(symTab,funcname);
                    //std::cout << "Type checking function call " << funcname << "\n";
                    if(entries.size() == 0) {
                        entries = getEntry(symTab,funcname,funccall->scopes);
                        if(entries.size() == 0) {
                            break;
                        }
                    }
                    SymbolTableEntry* e = entries.at(0);
                    auto funcparams = e->funcParams;
                    //auto args = funccall->margs;
                    auto args = funccall->mchildren;
                    std::vector<TypeInfo> arg_types;
                    arg_types.reserve(sizeof(TypeInfo)*args.size());
                    typeCheckPass(c,symTab);
                    resolveFunction(funccall,symTab);
                    for(auto a : args) {
                        //typeCheckPass(a,symTab);
                        auto tinfo = getTypeInfo(a,symTab);
                        arg_types.push_back(tinfo);
                    }

                    //check every arg, against every parameter
                    for(unsigned int i = 0; i < funcparams.size(); i++) {
                        //auto n = args.at(i);
                        auto param = funcparams.at(i).second;
                        auto paramt = param->mtypeinfo;
                        auto argt = arg_types.at(i);
                        //check compatibility
                        if(!isSameType(paramt,argt)) {
                            //check for casts
                            if(canCast(paramt,argt)) {
                                CastNode* cast = new CastNode();
                                cast->fromType = argt;
                                cast->toType = paramt;
                                cast->addChild(funccall->mchildren.at(i));
                                funccall->mchildren[i] = cast;
                            } else {
                                std::cout << "Error, cannot cast arg of type " << argt << " to " << paramt << '\n';
                            }
                        }
                    }
                }
                break;
            case ANT::Cast:
                {
                    //std::cout << __FILE__ << ':' << __FUNCTION__ << " Cast!\n";
                    auto cnode = (CastNode*)c;
                    typeCheckPass(c,symTab);
                    TypeInfo t = getTypeInfo(cnode->mchildren.at(0),symTab);
                    cnode->fromType = t;
                }
                break;
            case ANT::BinOp:
                {
                    //std::cout << __FILE__ << ':' << __FUNCTION__ << " BinOp!\n";
                    //TODO(marcus): support operator overloading
                    auto binopn = (BinOpNode*)c;
                    std::string op = binopn->getOp();

                    if(op.compare("( )") == 0) {
                        typeCheckPass(c,symTab);
                        TypeInfo t = getTypeInfo(binopn->LHS(),symTab);
                        binopn->mtypeinfo = t;
                    } else if(op.compare("@") == 0) {
                        typeCheckPass(c,symTab);
                        TypeInfo t = getTypeInfo(binopn->LHS(),symTab);
                        int err = decreaseDerefTypeInfo(t);
                        if(err) {
                            semanticError(SET::DerefNonPointer,c,symTab);
                        }
                        binopn->mtypeinfo = t;
                    } else if(op.compare(".") == 0) {
                        //TODO(marcus): fill out type checking for user structs
                        //get member name
                        if(binopn->RHS()->nodeType() != ANT::Var) {
                            std::cout << "RHS of . op was not a variable\n";
                            break;
                        }
                        typeCheckPass(c,symTab);
                        //FIXME(marcus): if . also dereferences then we will need to handle this case
                        //or maybe the right side is a dereferenced struct ptr, also
                        //will need to handle that case
                        auto lhs_t = getTypeInfo(binopn->LHS(),symTab);
                        auto structtypename = lhs_t.userid;
                        auto membername = ((VarNode*)binopn->RHS())->getVarName();
                        auto tmp = structList.find(structtypename);
                        if(tmp == structList.end()) std::cout << "Error, struct not found...\n";
                        auto strdef = tmp->second;
                        for(auto member : strdef->mchildren) {
                            auto vardec = (VarDecNode*)member;
                            auto var = (VarNode*)vardec->mchildren[0];
                            if(var->getVarName() == membername) {
                                binopn->mtypeinfo = var->mtypeinfo;
                                break;
                            }
                        }
                    } else if(op.compare("&") == 0) {
                        //TODO(marcus): make sure to differentiate between address-of and
                        //bitwise-and when you get both
                        typeCheckPass(c,symTab);
                        TypeInfo t = getTypeInfo(binopn->LHS(),symTab);
                        increaseDerefTypeInfo(t);
                        binopn->mtypeinfo = t;
                    } else if(op.compare("-") == 0 && binopn->unaryOp) {
                        //unary negation
                        typeCheckPass(binopn,symTab);
                        TypeInfo lhs_t = getTypeInfo(binopn->LHS(),symTab);
                        if(lhs_t.indirection) {
                            //TODO(marcus): get actual errors!
                            semanticError(SET::Unknown,binopn,symTab);
                        }
                        //TODO(marcus): actually type check
                        binopn->mtypeinfo = lhs_t;
                    } else if(op.compare("==") == 0 || op.compare("!=") == 0) {
                        typeCheckPass(binopn,symTab);
                        TypeInfo t;
                        t.type = SemanticType::Bool;
                        binopn->mtypeinfo = t;
                        TypeInfo lhs_t = getTypeInfo(binopn->LHS(),symTab);
                        TypeInfo rhs_t = getTypeInfo(binopn->RHS(),symTab);
                        //do compatiblity checking
                        if(!isSameType(lhs_t,rhs_t)) {
                            //check for casts
                            if(canCast(lhs_t,rhs_t)) {
                                CastNode* cast = new CastNode();
                                cast->fromType = rhs_t;
                                cast->toType = lhs_t;
                                cast->addChild(binopn->RHS());
                                binopn->setRHS(cast);
                            } else if(canCast(rhs_t,lhs_t)) {
                                CastNode* cast = new CastNode();
                                cast->fromType = lhs_t;
                                cast->toType = rhs_t;
                                cast->addChild(binopn->LHS());
                                binopn->setLHS(cast);
                            } else {
                                std::cout << "Error with use of operator " << op << '\n';
                                semanticError(SET::MissmatchBinop,binopn,symTab);
                            }
                        }
                    } else {
                        typeCheckPass(binopn,symTab);
                        //typeCheckPass(binopn->LHS(),symTab);
                        //typeCheckPass(binopn->RHS(),symTab);
                        TypeInfo lhs_t = getTypeInfo(binopn->LHS(),symTab);
                        TypeInfo rhs_t = getTypeInfo(binopn->RHS(),symTab);

                        //do compatiblity checking
                        if(isSameType(lhs_t,rhs_t)) {
                            binopn->mtypeinfo = lhs_t;
                            //binopn->mstype = lhs_t.type;
                        } else {
                            //check for pointer math
                            if(isPointerMath(lhs_t,rhs_t)) {
                                if(lhs_t.indirection) {
                                    binopn->mtypeinfo = lhs_t;
                                    //binopn->mstype = lhs_t.type;
                                } else {
                                    binopn->mtypeinfo = rhs_t;
                                    //binopn->mstype = rhs_t.type;
                                }
                            }
                            //check for casts
                            else if(canCast(lhs_t,rhs_t)) {
                                binopn->mtypeinfo = lhs_t;
                                CastNode* cast = new CastNode();
                                cast->fromType = rhs_t;
                                cast->toType = lhs_t;
                                cast->addChild(binopn->RHS());
                                binopn->setRHS(cast);
                            } else if(canCast(rhs_t,lhs_t)) {
                                binopn->mtypeinfo = rhs_t;
                                CastNode* cast = new CastNode();
                                cast->fromType = lhs_t;
                                cast->toType = rhs_t;
                                cast->addChild(binopn->LHS());
                                binopn->setLHS(cast);
                            } else {
                                std::cout << "Error with use of operator " << op << '\n';
                                semanticError(SET::MissmatchBinop,binopn,symTab);
                            }
                        }
                    }
                }

                break;
            case ANT::RetStmnt:
                {
                    //std::cout << __FILE__ << ':' << __FUNCTION__ << " Return!\n";
                    auto retn = (ReturnNode*)c;
                    TypeInfo ret_typeinfo;
                    ret_typeinfo.type = SemanticType::Void;
                    AstNode* expr = nullptr;
                    if(retn->mchildren.size() != 0) {
                        expr = retn->mchildren.at(0);
                        typeCheckPass(c,symTab);
                        ret_typeinfo = getTypeInfo(expr,symTab);
                    }
                    std::string name = ((FuncDefNode*)currentFunc)->mfuncname;
                    name += std::to_string(((FuncDefNode*)currentFunc)->id);
                    //std::string name = currentFunc2;
                    auto entries = getEntry(symTab,name);
                    //TypeInfo func_typeinfo = currentFunc2->mtypeinfo;
                    TypeInfo func_typeinfo = entries.at(0)->typeinfo;
                    //do compatibility checking
                    if(isSameType(func_typeinfo,ret_typeinfo)) {
                        //std::cout << "return type and function type matched!\n";
                    } else {
                        //check for casts
                        if(canCast(func_typeinfo,ret_typeinfo)) {
                            //std::cout << "return type and function type matched!\n";
                            CastNode* cast = new CastNode();
                            cast->fromType = ret_typeinfo;
                            cast->toType = func_typeinfo;
                            //NOTE(marcus): should never reach here with expr as nullptr
                            //can't cast void to another type.
                            cast->addChild(expr);
                            retn->mchildren[0] = cast;
                        } else {
                            retn->mtypeinfo = func_typeinfo;
                            semanticError(SemanticErrorType::MissmatchReturnType,retn,symTab);
                        }
                    }

                }
                break;
            case ANT::Assign:
                {
                    //std::cout << __FILE__ << ':' << __FUNCTION__ << " Assign!\n";
                    auto assignn = (AssignNode*)c;
                    auto lhs = assignn->getLHS();
                    auto rhs = assignn->getRHS();
                    typeCheckPass(c,symTab);
                    TypeInfo lhs_typeinfo = getTypeInfo(lhs,symTab);
                    TypeInfo rhs_typeinfo = getTypeInfo(rhs,symTab);
                    //std::cout << "All Type info gotten, beginning type checks\n";

                    //Do compatibility checking
                    if(!isSameType(lhs_typeinfo,rhs_typeinfo)) {
                        //check for casts
                        if(canCast(lhs_typeinfo,rhs_typeinfo)) {
                            CastNode* cast = new CastNode();
                            cast->fromType = rhs_typeinfo;
                            cast->toType = lhs_typeinfo;
                            cast->addChild(rhs);
                            assignn->mchildren[1] = cast;
                        } else {
                            semanticError(SET::MissmatchAssign,c,symTab);
                        }
                    }

                }
                break;
            case ANT::VarDecAssign:
                {
                    //std::cout << __FILE__ << ':' << __FUNCTION__ << " VarDecAssign!\n";
                    auto lhs = ((VarDecAssignNode*)c)->getLHS();
                    auto rhs = ((VarDecAssignNode*)c)->getRHS();
                    typeCheckPass(c, symTab);
                    TypeInfo lhs_typeinfo = getTypeInfo(c, symTab);
                    TypeInfo rhs_typeinfo = getTypeInfo(rhs,symTab);

                    if(lhs_typeinfo.type == SemanticType::Infer) {
                        //std::cout << "Inferred type!\n";
                        std::string varname = ((VarNode*)lhs)->getVarName();
                        //std::cout << "Inferring type for var " << varname << '\n';
                        //std::cout << rhs_typeinfo << '\n';
                        updateVarEntry(symTab,rhs_typeinfo,varname);
                        break;
                    }

                    //Do compatibility checking
                    if(!isSameType(lhs_typeinfo,rhs_typeinfo)) {
                    //check for casts
                        if(canCast(lhs_typeinfo,rhs_typeinfo)) {
                            //assignn->mtypeinfo = lhs_typeinfo;
                            CastNode* cast = new CastNode();
                            cast->fromType = rhs_typeinfo;
                            cast->toType = lhs_typeinfo;
                            cast->addChild(rhs);
                            ((VarDecAssignNode*)c)->mchildren[1] = cast;
                        } else {
                            semanticError(SET::MissmatchVarDecAssign,c,symTab);
                        }
                    }
                }
                break;
            case ANT::DeferStmt:
                typeCheckPass(c,symTab);
                break;
            case ANT::Prototype:
                {
                    //std::cout << __FILE__ << ':' << __FUNCTION__ << " Prototype!\n";
                    auto scope = getScope(symTab, ((PrototypeNode*)c)->mfuncname);
                    typeCheckPass(c,scope);
                }
                break;
            case ANT::Params:
                {
                    //std::cout << __FILE__ << ':' << __FUNCTION__ << " Params!\n";
                }
                break;
            case ANT::VarDec:
                //std::cout << __FILE__ << ':' << __FUNCTION__ << " VarDec!\n";
                break;
            case ANT::Var:
                //std::cout << __FILE__ << ':' << __FUNCTION__ << " Var!\n";
                break;
            case ANT::Const:
                //std::cout << __FILE__ << ':' << __FUNCTION__ << " Constant!\n";
                break;
            case ANT::StructDef:
                {
                    typeCheckPass(c,symTab);
                }
            default:
                //std::cout << __FILE__ << ':' << __FUNCTION__ << " Default Case!\n";
                break;
        }
    }
}

TypeInfo getTypeInfo(AstNode* ast, SymbolTable* symTab) {
    AstNodeType ast_node_type = ast->nodeType();
    switch(ast_node_type) {
        case ANT::Const:
            {
                return ((ConstantNode*)ast)->mtypeinfo;
            }
            break;
        case ANT::Var:
            {
                std::string name = ((VarNode*)ast)->getVarName();
                auto entries = getEntry(symTab,name);
                if(entries.size() > 0) {
                    SymbolTableEntry* e = entries.at(0);
                    ast->mtypeinfo = e->typeinfo;
                    return e->typeinfo;
                } else {
                    std::cout << "No Entries! Var name is " << name << "\n";
                }
            }
            break;
        case ANT::VarDec:
            {
                std::string name = ((VarNode*)((VarDecNode*)ast)->getLHS())->getVarName();
                auto entries = getEntry(symTab,name);
                SymbolTableEntry* e = entries.at(0);
                return e->typeinfo;
            }
            break;
        case ANT::VarDecAssign:
            {
                std::string name = ((VarNode*)((VarDecAssignNode*)ast)->getLHS())->getVarName();
                auto entries = getEntry(symTab,name);
                SymbolTableEntry* e = entries.at(0);
                return e->typeinfo;
            }
            break;
        case ANT::FuncCall:
            {
                //TODO(marcus): will have to deal with overloads somehow
                std::string name = ((FuncCallNode*)ast)->mfuncname;
                auto entries = getEntry(symTab,name);
                if(entries.size() == 0) {
                    entries = getEntry(symTab,name,((FuncCallNode*)ast)->scopes);
                }
                SymbolTableEntry* e = entries.at(0);
                return e->node->mtypeinfo;
                //TODO(marcus): function entries don't get their typeinfo field set
                return e->typeinfo;
            }
            break;
        case ANT::BinOp:
            {
                auto binopn = (BinOpNode*)ast;
                return binopn->mtypeinfo;
            }
        case ANT::Cast:
            {
                auto castnode = (CastNode*)ast;
                return castnode->toType;
            }
        default:
            break;
    }
    //TODO(marcus): add an error type
    TypeInfo error;
    return error;
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
                    //std::cout << __FILE__ << ':' << __FUNCTION__ << " FunctionDef!\n";
                    std::string name = ((FuncDefNode*)c)->mfuncname + std::to_string(((FuncDefNode*)c)->id);
                    //std::cout << "Entering into scope " << name << "\n";
                    auto scope = getScope(symTab, name);
                    variableUseCheck(c,scope);
                }
                break;
            case ANT::Block:
                {
                    //std::cout << __FILE__ << ':' << __FUNCTION__ << " Block!\n";
                    auto scope = getScope(symTab, "block"+std::to_string(((BlockNode*)c)->getId()));
                    //std::cout << "Entering into scope " << scope->name << "\n";
                    variableUseCheck(c,scope);
                }
                break;
            case ANT::IfStmt:
                {
                    //std::cout << __FILE__ << ':' << __FUNCTION__ << " If!\n";
                    variableUseCheck(c,symTab);
                }
                break;
            case ANT::ForLoop:
                {
                    //std::cout << __FILE__ << ':' << __FUNCTION__ << " For!\n";
                    auto scope = getScope(symTab, "for"+std::to_string(((ForLoopNode*)c)->getId()));
                    //std::cout << "Entering into scope " << scope->name << '\n';
                    variableUseCheck(c,scope);
                }
                break;
            case ANT::WhileLoop:
                {
                    //std::cout << __FILE__ << ':' << __FUNCTION__ << " While!\n";
                    auto scope = getScope(symTab, "while"+std::to_string(((WhileLoopNode*)c)->getId()));
                    //std::cout << "Entering into scope " << scope->name << '\n';
                    variableUseCheck(c,scope);
                }
                break;
            case ANT::Assign:
                {
                    //std::cout << __FILE__ << ':' << __FUNCTION__ << " Assign!\n";
                    variableUseCheck(c,symTab);
                }
                break;
            case ANT::VarDec:
                {
                    //std::cout << __FILE__ << ':' << __FUNCTION__ << " VarDec!\n";
                    //std::cout << "SymbolTable " << symTab->name << "\n";
                    std::string name = ((VarNode*)(c->getChildren()->at(0)))->getVarName();
                    auto entry = getEntryCurrentScope(symTab,name);
                    if(entry) {
                        semanticError(SET::DupDecl, c->getChildren()->at(0), symTab);
                    } else {
                        ////std::cout << "Adding variable declaration entry!\n";
                        TypeInfo typeinfo = ((VarNode*)(c->getChildren()->at(0)))->mtypeinfo;
                        //TODO(marcus): get struct name if the var dec is a user type
                        addVarEntry(symTab, typeinfo, name);
                        //addVarEntry(symTab, SemanticType::Typeless, c->getChildren()->at(0));
                    }
                }
                break;
            case ANT::VarDecAssign:
                {
                    //std::cout << __FILE__ << ':' << __FUNCTION__ << " VarDecAssign!\n";
                    std::string name = ((VarNode*)(c->getChildren()->at(0)))->getVarName();
                    auto entry = getEntryCurrentScope(symTab,name);
                    if(entry) {
                        semanticError(SET::DupDecl, c->getChildren()->at(0), symTab);
                    } else {
                        ////std::cout << "Adding variable declaration entry!\n";
                        TypeInfo typeinfo = ((VarNode*)(c->getChildren()->at(0)))->mtypeinfo;
                        if(typeinfo.type == SemanticType::Typeless) typeinfo.type = SemanticType::Infer;
                        //TODO(marcus): add userid if type is a struct!
                        addVarEntry(symTab, typeinfo, name);
                        //addVarEntry(symTab, SemanticType::Typeless, c->getChildren()->at(0));
                        auto rhs = ((VarDecAssignNode*)c)->getRHS();
                        variableUseCheck(rhs,symTab);
                    }
                }
                break;
            case ANT::BinOp:
                {
                    //std::cout << __FILE__ << ':' << __FUNCTION__ << " BinOp!\n";
                    auto expr = (BinOpNode*)c;
                    variableUseCheck(expr,symTab);
                }
                break;
            case ANT::Var:
                {
                    //std::cout << __FILE__ << ':' << __FUNCTION__ << " Var!\n";
                    std::string name = ((VarNode*)(c))->getVarName();
                    auto entry = getEntry(symTab,name);
                    if(entry.size() == 0) {
                        //NOTE(marcus): once we have type information we need to check that the
                        //member we want actually exists for that type
                        bool isStructMember = false;
                        for(auto& struct_t : userTypesList) {
                            auto members = struct_t.second;
                            if(members->find(name) != members->end()) {
                                isStructMember = true;
                                break;
                            }
                        }
                        if(!isStructMember) {
                            semanticError(SET::UndefUse, c, symTab);
                        }
                    } else {
                        //std::cout << "Var Use Check, entry size is... " << entry.size() << '\n';
                    }
                }
                break;
            case ANT::FuncCall:
                {
                    //std::cout << __FILE__ << ':' << __FUNCTION__ << " FuncCall!\n";
                    std::string name = ((FuncCallNode*)c)->mfuncname;
                    //std::cout << "Looking in symboltable for function " << name << "\n";
                    auto entry = getEntry(symTab,name);
                    if(entry.size() == 0) {
                        entry = getEntry(symTab,name,((FuncCallNode*)c)->scopes);
                        if(entry.size() == 0) {
                            semanticError(SET::NoFunc, c, symTab);
                        }
                    } else {
                        //std::cout << "Function found\n";
                    }
                    variableUseCheck(c,symTab);
                }
                break;
            case ANT::Params:
                {
                    //std::cout << __FILE__ << ':' << __FUNCTION__ << " Params!\n";
                    auto param_node = (ParamsNode*)c;
                    auto name = param_node->mname;
                    auto entry = getEntry(symTab,name);
                    if(entry.size() != 0) {
                        //std::cout << "Function param " << name << " was previously declared\n";
                    } else {
                        //std::cout << "Adding entry to symbol table, Param " << name << '\n';
                        TypeInfo param_typeinfo = param_node->mtypeinfo;
                        //std::cout << "Param type info: " << param_typeinfo << '\n';
                        addVarEntry(symTab,param_typeinfo,name);
                    }
                }
                break;
            case ANT::DeferStmt:
                {
                    //std::cout << __FILE__ << ':'<< __FUNCTION__ << " Defer!\n";
                    variableUseCheck(c,symTab);
                }
                break;
            case ANT::RetStmnt:
                {
                    //std::cout << __FILE__ << ':'<< __FUNCTION__ << " Return!\n";
                    variableUseCheck(c,symTab);
                }
                break;
            case ANT::Prototype:
                {
                    //std::cout << __FILE__ << ':'<< __FUNCTION__ << " Prototype!\n";
                    std::string name = ((PrototypeNode*)c)->mfuncname;
                    //std::cout << "Entering into scope " << name << "\n";
                    auto scope = getScope(symTab, name);
                    variableUseCheck(c,scope);
                }
                break;
            case ANT::Type:
            case ANT::Const:
                break;
            default:
                //std::cout << "Default Case Variable Use Check\n";
                break;
        }
    }
}

void printSymbolTable() {
    printTable(&progSymTab);
}

std::vector<std::vector<AstNode*>> deferStacks;
void printDeferStacks() {
    std::cout << "deferStacks.size = " << deferStacks.size() << "\n";
    std::cout << "Sizes: ";
    for(auto& s : deferStacks) {
        std::cout << s.size() << " ";
    }
    std::cout << "\n";
}
void deferPass(AstNode* ast) {
    auto children = ast->getChildren();
    for(auto i = children->begin(); i != children->end(); i++) {
        AstNode* c = *i;
        AstNodeType nodetype = c->nodeType();
        if(nodetype == AstNodeType::FuncDef) {
            deferPass(c);
        } else if(nodetype == AstNodeType::Block) {
            //std::cout << "Block Encountered, adding new stack\n";
            deferStacks.push_back(std::vector<AstNode*>());
            deferPass(c);
        } else if(nodetype == AstNodeType::DeferStmt) {
            //std::cout << "Defer Statement! adding to stack!\n";
            deferStacks.back().push_back(c);
            //printDeferStacks();
            deferPass(c);
            //remove defer from list of children
            i = children->erase(i);
            i--;
            //if(i != children->end()) {
            //    i--;
            //}
        } else if(nodetype == AstNodeType::LoopStmt) {
            //end of loop scope, iterate through all defer statements in last stack
            if(deferStacks.size() > 0) {
                auto defers = deferStacks.back();
                for(auto deferstmt = defers.rbegin(); deferstmt != defers.rend(); deferstmt++) {
                    auto defer_node = (DeferStmtNode*)*deferstmt;
                    //TODO(marcus): don't hardcode child access
                    auto to_insert = defer_node->mchildren.back();
                    i = children->insert(i,to_insert);
                    i++;
                }
            }
        } else if(nodetype == AstNodeType::RetStmnt) {
            //std::cout << "deferPass: Return Statement Found\n";
            //printDeferStacks();
            //iterate thru all defer stacks, last to first
            for(auto stack_iter = deferStacks.rbegin(); stack_iter != deferStacks.rend(); stack_iter++) {
                auto def_stack = stack_iter;
                for(auto deferstmt = def_stack->rbegin(); deferstmt != def_stack->rend(); deferstmt++) {
                    auto defer_node = (DeferStmtNode*)*deferstmt;
                    //if(defer_node == nullptr) { //std::cout << "ERROR!!! NULLPTR!!!\n"; }
                    //TODO(marcus): don't hardcode child access
                    ////std::cout << defer_node->mchildren.size() << "IS THE SIZE\n";
                    auto to_insert = defer_node->mchildren.back();
                    i = children->insert(i,to_insert);
                    i++;

                }
            }
        } else if(nodetype == AstNodeType::CompileUnit) {
            deferPass(c);
        } else {
            continue;
        }
    }
    if(ast->nodeType() == AstNodeType::Block) {
        //std::cout << "deferPass: End of Block is here!\n";
        //printDeferStacks();
        //end of block scope, iterate through all defer statements in last stack
        if(deferStacks.size() > 0) {
            auto defers = deferStacks.back();
            for(auto deferstmt = defers.rbegin(); deferstmt != defers.rend(); deferstmt++) {
                auto defer_node = (DeferStmtNode*)*deferstmt;
                //TODO(marcus): don't hardcode child access
                auto to_insert = defer_node->mchildren.back();
                auto last_iter = children->end()--;
                children->insert(last_iter,to_insert);
            }
            //remove last list
            //TODO(marcus): we have a memory leak here of all the defer nodes.
            //we can fix this using a custom allocator for defer nodes.
            auto last_stack = --deferStacks.end();
            //std::cout << "erasing last stack\n";
            deferStacks.erase(last_stack);
            //printDeferStacks();
        }

    }
    return;
}

#define ST SemanticType

static int calcTypeSize(TypeInfo t) {
//TODO(marcus): make this not hard coded
    if(t.indirection) {
        return 8;
    }

    switch(t.type) {
        case ST::Bool:
        case ST::Char:
        case ST::u8:
        case ST::s8:
            return 1;
            break;
        case ST::u16:
        case ST::s16:
            return 2;
            break;
        case ST::u32:
        case ST::s32:
        case ST::Float:
        case ST::Int:
        case ST::intlit:
        case ST::floatlit:
            return 4;
            break;
        case ST::u64:
        case ST::s64:
        case ST::Double:
            return 8;
            break;
        case ST::User:
/*
 *            {
 *                int sum = 0;
 *                //TODO(marcus): factor in alignment?
 *                auto iter = userTypesList.find(t.userid);
 *                if(iter == userTypesList.end()) {
 *                    //TODO(marcus): add error case
 *                    semanticError(SemanticErrorType::Unknown, t,t);
 *                    return 1;
 *                }
 *
 *                auto members = iter->second;
 *
 *                for(auto i : *members) {
 *                    s
 *                }
 *                return sum;
 *            }
 *            break;
 */
            //TODO(marcus): implement this for user defined types!
            return 1;
        default:
            //TODO(marcus): fix this error
            semanticError(SemanticErrorType::Unknown, nullptr, &progSymTab);
            return 0;
            break;
    }
}

#undef ST

void resolveSizeOfs(AstNode* ast) {
    std::vector<AstNode*>* vec = ast->getChildren();
    for(unsigned int i = 0; i < vec->size(); i++) {
        AstNode* child = (*vec)[i];
        if(child->nodeType() == ANT::SizeOf) {
            int size = calcTypeSize(child->mtypeinfo);
            auto val = std::to_string(size);
            auto cnode = new ConstantNode();
            cnode->setVal(val);
            cnode->mtypeinfo.type = SemanticType::intlit;
            (*vec)[i] = cnode;
            delete child; //TODO(marcus): allocate all sizeofs with a special allocator
        }
    }
    for(auto c : (*vec)) {
        resolveSizeOfs(c);
    }
}

void checkAssignments(AstNode* ast) {
    checkAssignments(ast,&progSymTab);
}

void checkAssignments(AstNode* ast, SymbolTable* symTab) {
    std::vector<AstNode*>* vec = ast->getChildren();
    auto scope = symTab;
    for(unsigned int i = 0; i < vec->size(); i++) {
        AstNode* child = (*vec)[i];
        if(child->nodeType() == ANT::Assign) {
            auto anode = (AssignNode*)child;
            auto lhs = anode->getLHS();
            if(lhs->nodeType() == ANT::Var) {
                continue;
            }
            if(lhs->nodeType() == ANT::BinOp) {
                auto binopn = (BinOpNode*)lhs;
                auto op = binopn->mop;
                if((op == "@") || (op == ".")) {
                    continue;
                }
            }
            semanticError(SemanticErrorType::NotLValue,child,symTab);
        } else if(child->nodeType() == ANT::BinOp) {
            auto binopn = (BinOpNode*)child;
            auto op = binopn->mop;
            if(op == ".") {
                auto lhs = binopn->LHS();
                auto lhst = lhs->nodeType();
                if(lhst == ANT::Var || lhst == ANT::FuncCall || lhst == ANT::BinOp) {
                    continue;
                } else {
                    //Error
                    semanticError(SemanticErrorType::DotOpLhs,binopn, symTab);
                }
            }
        } else {
            switch(child->nodeType()) {
                case ANT::CompileUnit:
                    scope = getScope(symTab, ((CompileUnitNode*)child)->getFileName());
                    break;
                case ANT::FuncDef:
                    scope = getScope(symTab, ((FuncDefNode*)child)->mfuncname + std::to_string(((FuncDefNode*)child)->id));
                    break;
                case ANT::Block:
                    scope = getScope(symTab, "block"+std::to_string(((BlockNode*)child)->getId()));
                    break;
                case ANT::ForLoop:
                    scope = getScope(symTab, "for"+std::to_string(((ForLoopNode*)child)->getId()));
                    break;
                case ANT::WhileLoop:
                    scope = getScope(symTab, "while"+std::to_string(((WhileLoopNode*)child)->getId()));
                    break;
                default:
                    break;
            }
        }
    }
    for(auto c : (*vec)) {
        checkAssignments(c, scope);
    }
}

bool resolveFunction(FuncCallNode* funccall, SymbolTable* symTab) {
    auto funcname = funccall->mfuncname;
    auto entries = getEntry(symTab,funcname);

    if(entries.size() == 0) {
        entries = getEntry(symTab,funcname,funccall->scopes);
        if(entries.size() == 0) {
            semanticError(SemanticErrorType::NoFunction, funccall, symTab);
            return false;
        }
    }
    auto e = entries.at(0);

    //TODO(marcus): Shouold we allow mix of prototypes and mangled funcs of same name?
    //eg extern foo() and foo()
    if(e->node->nodeType() == AstNodeType::Prototype) {
        auto funcparams = ((PrototypeNode*)e->node)->getParameters();
        if(funcparams->size() != funccall->mchildren.size()) {
            std::cout << "Missmatched number of parameters. At Callsite: " << funccall->mchildren.size() << " Function: " << funcparams->size() << '\n';
            delete funcparams;
            return false;
        }
        bool matched = true;

        //check every arg, against every parameter
        for(unsigned int i = 0; i < funcparams->size(); i++) {
            auto param = funcparams->at(i);
            auto paramt = param->mtypeinfo;
            auto argn = funccall->mchildren.at(i);
            auto argt = getTypeInfo(argn,symTab);

            if(!isSameType(paramt,argt)) {
                if(!canCast(paramt,argt)) {
                    std::cout << "types different " << paramt << " and " << argt << '\n';
                    matched = false;
                    break;
                }
            }
        }
        if(!matched) {
            semanticError(SemanticErrorType::NoResolve, funccall, symTab);
            return false;
        }
        delete funcparams;
        return true;
    }

    FuncDefNode* matchedNode = nullptr;
    for(auto overload : e->overloads) {
        FuncDefNode* candidate = nullptr;
        if(overload->nodeType() == AstNodeType::FuncDef) {
            candidate = (FuncDefNode*)overload;
        }
        auto funcparams = candidate->getParameters();

        if(funcparams->size() != funccall->mchildren.size()) {
            delete funcparams;
            continue;
        }

        bool matched = true;

        //check every arg, against every parameter
        for(unsigned int i = 0; i < funcparams->size(); i++) {
            auto param = funcparams->at(i);
            auto paramt = param->mtypeinfo;
            auto argn = funccall->mchildren.at(i);
            auto argt = getTypeInfo(argn,symTab);

            if(!isSameType(paramt,argt)) {
                if(!canCast(paramt,argt)) {
                    matched = false;
                    break;
                }
            }
        }
        if(matched) {
            if(!matchedNode) {
                matchedNode = candidate;
            } else {
                semanticError(SemanticErrorType::MultipleFuncResolve, funccall, symTab);
                //std::cout << "Error. Multiple matching functions found.\n";
            }
        }
        delete funcparams;
    }

    if(matchedNode) {
        auto mangled = matchedNode->mangledName();
        //std::cout << "Matched: " << mangled << '\n';
        //std::cout << "Replacing function call name\n";
        funccall->mfuncnamemangled = mangled;
    } else {
        semanticError(SemanticErrorType::NoResolve, funccall, symTab);
    }
    return (matchedNode != nullptr);
}

static void importPrepass(AstNode* ast, SymbolTable* symTab) {
    auto compileunit = (CompileUnitNode*)ast;
    auto projectSymtab = symTab->parent;
    for(auto& i : compileunit->imports) {
        auto child = projectSymtab->children.find(i);
        if(child != projectSymtab->children.end()) {
            //std::cout << "Adding " << i << " to the symbol table\n";
            symTab->imports.insert(std::make_pair(i,child->second));
        } else {
            std::cout << "Didn't find " << i << " imported\n";
        }
    }
}

void importPrepass(AstNode* ast) {
    for(auto c : (*(ast->getChildren()))) {
        switch(c->nodeType()) {
            case ANT::CompileUnit:
                {
                auto scope = getScope(&progSymTab, ((CompileUnitNode*)c)->getFileName());
                importPrepass(c, scope);
                }
                break;
            default:
                //std::cout << "unknown ast node type!\n";
                break;
        }
    }
}

void transformAssignments(AstNode* ast) {
    for(auto c : (*(ast->getChildren()))) {
        switch(c->nodeType()) {
            case ANT::Assign:
                {
                    auto token = c->mtoken;
                    auto t = token.type;
                    auto lhs = ((AssignNode*)c)->getLHS();
                    auto rhs = ((AssignNode*)c)->getRHS();

                    if(t == TokenType::assignment)
                        break;

                    auto bop = new BinOpNode();
                    bop->mtoken = token;
                    if(t == TokenType::addassign) {
                        bop->setOp("+");
                    } else if(t == TokenType::subassign) {
                        bop->setOp("-");
                    } else if(t == TokenType::mulassign) {
                        bop->setOp("*");
                    } else if(t == TokenType::divassign) {
                        bop->setOp("/");
                    }

                    bop->addChild(lhs);
                    bop->addChild(rhs);
                    c->mchildren[1] = bop;
                }
                break;
            default:
                transformAssignments(c);
                break;
        }
    }
}

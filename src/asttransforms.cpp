#include "asttransforms.h"
#include "astnodetypes.h"
#include "astnode.h"
#include "symboltable.h"
#include "coloroutput.h"
#include <iostream>
#include <string>
#include <utility>
#include <cstring>
#include <algorithm>
#include <set>
#include <stdio.h>
#include <assert.h>

extern bool semantic_error;

//TODO(marcus): should this be global? is there a better way?
SymbolTable progSymTab;

//TODO(marcus): Currently this is global. Is there a better way to do this? Maybed a list of types
//per file?
//a list of user defined types with a mapping of member name to member index
std::unordered_map<std::string,std::unordered_map<std::string,int>*> userTypesList;
std::unordered_map<std::string,AstNode*> structList;
std::vector<AstNode*> templateList;
std::vector<StructDefNode*> templatedStructList;
std::vector<FuncDefNode*> instantiatedFunctionsList;
std::vector<StructDefNode*> instantiatedStructList;

//TODO(marcus): put this in the SymbolTable?
std::unordered_map<std::string,std::vector<FuncDefNode*>> operatorOverloads;
static const char* getOperatorName(const char* op);

#define ANT AstNodeType
#define SET SemanticErrorType
static void typeCheckPass(AstNode* ast, SymbolTable* symTab);
static void populateSymbolTableFunctions(AstNode* ast, SymbolTable* symTab);
TypeInfo getTypeInfo(AstNode* ast, SymbolTable* symTab);
static bool isSameType(const TypeInfo& t1, const TypeInfo& t2);
static void checkForRecursiveTypes();
static AstNode* instantiateTemplate(FuncCallNode* funccall, FuncDefNode* funcdef, SymbolTable* symTab);
static int calcTypeSize(const TypeInfo& t);

void semanticPass1(AstNode* ast, int loopDepth, SymbolTable* symTab)
{
    /*
     * This pass checks for:
     * continue/break being used outside of a loop
     * initial check that all assignments have lvalues
     * all variables are used before they are defined
     * that the . op lhs is a variable, binop, or function call
     */
    AstNodeType type = ast->nodeType();
    int nextLoopDepth = loopDepth;

    auto scope = symTab;
    switch(type) {
        case ANT::CompileUnit:
            scope = getScope(symTab, static_cast<CompileUnitNode*>(ast)->getFileName());
            break;
        case ANT::FuncDef:
            scope = getScope(symTab, static_cast<FuncDefNode*>(ast)->mfuncname + std::to_string(static_cast<FuncDefNode*>(ast)->id));
            break;
        case ANT::Prototype:
            {
                std::string name = static_cast<FuncDefNode*>(ast)->mfuncname;
                scope = getScope(symTab, name);
            }
            break;
        case ANT::Block:
            scope = getScope(symTab, "block"+std::to_string(static_cast<BlockNode*>(ast)->getId()));
            break;
        case ANT::ForLoop:
            nextLoopDepth += 1;
            scope = getScope(symTab, "for"+std::to_string(static_cast<LoopNode*>(ast)->getId()));
            break;
        case ANT::WhileLoop:
            nextLoopDepth += 1;
            scope = getScope(symTab, "while"+std::to_string(static_cast<LoopNode*>(ast)->getId()));
            break;
        case ANT::LoopStmtBrk:
        case ANT::LoopStmtCnt:
            if(loopDepth == 0) {
                semanticError(SET::OutLoop, ast, symTab);
            }
            break;
        case ANT::Var:
            {
                std::string name = static_cast<VarNode*>(ast)->getVarName();
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
                        bool isTemplateStructMember = false;
                        for(auto struct_t : templatedStructList) {
                            for(auto c : struct_t->mchildren) {
                                if(c->nodeType() != ANT::VarDec) {
                                    continue;
                                }
                                auto vdec = static_cast<VarDeclNode*>(c);
                                auto v = static_cast<VarNode*>(vdec->getLHS());
                                std::string member_name = v->getVarName();
                                if(member_name == name) {
                                    isTemplateStructMember = true;
                                    break;
                                }
                            }
                        }
                        if(!isTemplateStructMember) {
                            semanticError(SET::UndefUse, ast, symTab);
                        }
                    }
                }
            }
            break;
        case ANT::UnionDef:
        case ANT::StructDef:
            {
                //TODO(marcus): we return here because we don't want struct var decls to be added to
                //the symbol table, this should be fixed another way though.
                return;
            }
        case ANT::VarDec:
            {
                std::string name = static_cast<VarNode*>(ast->getChildren()->at(0))->getVarName();
                auto entry = getEntryCurrentScope(symTab,name);
                if(entry) {
                    semanticError(SET::DupDecl, ast->getChildren()->at(0), symTab);
                } else {
                    TypeInfo typeinfo = static_cast<VarNode*>(ast->getChildren()->at(0))->mtypeinfo;
                    addVarEntry(symTab, typeinfo, name);
                }
            }
            break;
        case ANT::VarDecAssign:
            {
                std::string name = static_cast<VarNode*>(ast->getChildren()->at(0))->getVarName();
                auto entry = getEntryCurrentScope(symTab,name);
                if(entry) {
                    semanticError(SET::DupDecl, ast->getChildren()->at(0), symTab);
                } else {
                    TypeInfo typeinfo = static_cast<VarNode*>(ast->getChildren()->at(0))->mtypeinfo;
                    if(typeinfo.type == SemanticType::Typeless) typeinfo.type = SemanticType::Infer;
                    addVarEntry(symTab, typeinfo, name);
                    auto rhs = static_cast<VarDeclNode*>(ast)->getRHS();
                    semanticPass1(rhs, nextLoopDepth, symTab);
                    return;
                }
            }
            break;
        case ANT::Params:
            {
                auto param_node = static_cast<ParamsNode*>(ast);
                auto name = param_node->mname;
                auto entry = getEntry(symTab,name);
                if(entry.size() != 0) {
                    //TODO(marcus): make this an error
                    std::cout << "Function param " << name << " was previously declared\n";
                } else {
                    TypeInfo param_typeinfo = param_node->mtypeinfo;
                    addVarEntry(symTab,param_typeinfo,name);
                }
            }
            break;
        case ANT::FuncCall:
            {
                std::string name = static_cast<FuncCallNode*>(ast)->mfuncname;
                auto entry = getEntry(symTab,name);
                if(entry.size() == 0) {
                    entry = getEntry(symTab,name,static_cast<FuncCallNode*>(ast)->scopes);
                    if(entry.size() == 0) {
                        bool callToTemplate = false;
                        for(auto ptr : templateList) {
                            auto fdef = static_cast<FuncDefNode*>(ptr);
                            if(fdef->mfuncname == name) {
                                callToTemplate = true;
                                break;
                            }
                        }
                        if(callToTemplate) break;
                        semanticError(SET::NoFunc, ast, symTab);
                    }
                }
            }
            break;
        case ANT::Assign:
            {
                auto anode = static_cast<AssignNode*>(ast);
                auto lhs = anode->getLHS();
                if(lhs->nodeType() == ANT::Var) {
                    break;
                }
                if(lhs->nodeType() == ANT::BinOp || lhs->nodeType() == ANT::UnaryOp) {
                    auto binopn = static_cast<BinOpNode*>(lhs);
                    auto op = binopn->mop;
                    if(std::strcmp(op,"@") == 0 || std::strcmp(op,".") == 0) {
                        break;
                    }
                }
                semanticError(SemanticErrorType::NotLValue,ast,symTab);
            }
        case ANT::UnaryOp:
        case ANT::BinOp:
            {
                auto binopn = static_cast<BinOpNode*>(ast);
                auto op = binopn->mop;
                if(std::strcmp(op,".") == 0) {
                    auto lhs = binopn->LHS();
                    auto lhst = lhs->nodeType();
                    if(lhst == ANT::Var || lhst == ANT::FuncCall || lhst == ANT::BinOp || lhst == ANT::UnaryOp) {
                        break;
                    } else {
                        //Error
                        semanticError(SemanticErrorType::DotOpLhs,binopn, symTab);
                    }
                }
            }
        default:
            break;
    }
    for(auto c : (*(ast->getChildren()))) {
        semanticPass1(c, nextLoopDepth, scope);
    }
    return;
}

void semanticPass1(AstNode* ast) 
{
    semanticPass1(ast,0,&progSymTab);
}

static void registerTypeDef(StructDefNode* n) {
    auto type_name = n->ident;
    if(n->isTemplated) {
        templatedStructList.push_back(n);
        return;
    }

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
        auto vdec = static_cast<VarDeclNode*>(c);
        auto v = static_cast<VarNode*>(vdec->getLHS());
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
                {
                    auto scope = addNewScope(sym, static_cast<CompileUnitNode*>(c)->getFileName());
                    populateTypeList(c, scope);
                }
                break;
            case ANT::UnionDef:
            case ANT::StructDef:
                //parse definition!
                registerTypeDef(static_cast<StructDefNode*>(c));
                break;
            default:
                continue;
                break;
        }
    }
}
void populateTypeList(AstNode* ast) {
    populateTypeList(ast,&progSymTab);
    checkForRecursiveTypes();
}

void populateSymbolTableFunctions(AstNode* ast) {
    populateSymbolTableFunctions(ast,&progSymTab);
}

SymbolTable* getSymtab(const std::string& file) {
    return progSymTab.children.find(file)->second;
}

static void populateSymbolTableFunctions(AstNode* ast, SymbolTable* symTab) {
    for(auto c : (*(ast->getChildren()))) {
        std::vector<std::pair<TypeInfo,AstNode*>> p;
        SemanticType retType;
        switch(c->nodeType()) {
            case ANT::CompileUnit:
                {
                    auto scope = addNewScope(symTab, static_cast<CompileUnitNode*>(c)->getFileName());
                    populateSymbolTableFunctions(c, scope);
                }
                break;
            case ANT::Prototype:
                {
                    auto proto = static_cast<FuncDefNode*>(c);
                    retType = proto->getType();
                    auto params = proto->getParameters();
                    for(auto prm : (params)) {
                        auto prmti = prm->mtypeinfo;
                        p.push_back(std::make_pair(prmti,prm));
                    }
                    addFuncEntry(symTab, retType, c, p);
                }
                break;
            case ANT::FuncDef:
                {
                    auto func = static_cast<FuncDefNode*>(c);
                    if(func->isTemplated) {
                        templateList.push_back(func);
                        break;
                    }

                    if(func->isOperatorOverload == 1) {
                        auto op = func->op;
                        std::string opstr = getOperatorName(op);
                        opstr = "op_"+opstr;
                        //std::cout << "op is ... " << opstr << '\n';
                        operatorOverloads.emplace(opstr,std::vector<FuncDefNode*>());
                        operatorOverloads[opstr].push_back(func);
                        func->addFuncName(opstr);
                    }

                    retType = func->getType();
                    auto params = func->getParameters();
                    auto entries = getEntry(symTab,func->mfuncname);
                    if(entries.size() != 0) {
                        auto e = entries.at(0);
                        auto numParams = params.size;
                        //For every overload
                        for(auto ol : e->overloads) {
                            if(numParams == ol->mchildren.size()-1) {
                                bool repeat = true;
                                //check for any mismatched param types
                                for(unsigned int i = 0; i < numParams; ++i) {
                                    if(!isSameType(params.ptr[i]->mtypeinfo,ol->mchildren.at(i)->mtypeinfo)) {
                                        repeat = false;
                                        break;
                                    }
                                }
                                if(!isSameType(func->mtypeinfo,ol->mtypeinfo)) {
                                    repeat = false;
                                }
                                if(repeat) {
                                    semanticError(SemanticErrorType::DupFuncDef, ol, symTab);
                                }
                            }
                        }
                    }
                    for(auto prm : (params)) {
                        auto prmti = prm->mtypeinfo;
                        p.push_back(std::make_pair(prmti,prm));
                    }
                    addFuncEntry(symTab, retType, c, p);
                    addFuncEntry(symTab, func);
                }
                break;
            default:
                break;
        }
    }
}

void typeCheckPass(AstNode* ast) {
    //std::cout << "Type Checking Pass\n";
    for(auto c : (*(ast->getChildren()))) {
        switch(c->nodeType()) {
            case ANT::CompileUnit:
                {
                auto scope = addNewScope(&progSymTab, static_cast<CompileUnitNode*>(c)->getFileName());
                typeCheckPass(c, scope);
                }
                break;
            default:
                //std::cout << "unknown ast node type!\n";
                break;
        }
    }

}

static bool isSameType(const TypeInfo& t1, const TypeInfo& t2) {
    if(t1.indirection() == t2.indirection()) {
        if(t1.type == t2.type) {
            if(t1.type != SemanticType::User) {
                return true;
            } else {
                return (std::strcmp(t1.userid,t2.userid) == 0);
            }
        }
    }
    return false;
}

#define ST SemanticType
bool isPointerMath(const TypeInfo& t1, const TypeInfo& t2) {
    if(t1.indirection() > 0 && t2.indirection() > 0) {
        return false;
    }

    SemanticType st = ST::Typeless;
    if(t1.indirection() > 0) {
        st = t2.type;
    } else if(t2.indirection() > 0) {
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

static bool isIntegerSigned(const TypeInfo& t)
{
    switch(t.type) {
        case ST::u8:
        case ST::u16:
        case ST::u32:
        case ST::u64:
            return false;
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

static bool isIntegerType(const TypeInfo& t)
{
    if(t.indirection() || t.arraySize()) return false;
    switch(t.type) {
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

static bool canCast(const TypeInfo& t1, const TypeInfo& t2) {
    //helper function to check if we can implicitly cast t2 to the t1

    if((t1.type == SemanticType::nulllit && t2.indirection()) || (t2.type == SemanticType::nulllit && t1.indirection())) {
        return true;
    }

    if(t1.type == SemanticType::User && t2.type == SemanticType::User) {
        return (std::strcmp(t1.userid,t2.userid) == 0);
    }

    if(t1.indirection() != t2.indirection()) {
        //can't implicitly convert two diff pointer types
        return false;
    }

    if(t1.indirection() == 1 && t2.indirection() == 1) {
        if(t1.type == SemanticType::u8 || t1.type == SemanticType::Char) {
            if(t2.type == SemanticType::u8 || t2.type == SemanticType::Char) {
                return true;
            }
        }
    }

    if(t1.indirection() > 0 || t2.indirection() > 0) {
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

    if(isIntegerType(t2) && isIntegerType(t1)) {
        int t1_size = calcTypeSize(t1);
        int t2_size = calcTypeSize(t2);
        bool t1_signed = isIntegerSigned(t1);
        bool t2_signed = isIntegerSigned(t2);
        if(t1_signed == t2_signed)
        {
            //If their signs match, then accept as long as resulting size is >= current
            return (t1_size >= t2_size);
        } else {
            //signed to unsigned cast is not allowed
            if(t2_signed && !(t1_signed)) {
                if(t2.type == ST::intlit) return true;
                return false;
            }
            //unsigned to signed cast allowed if the signed size is larger
            if(!(t2_signed) && t1_signed) {
                if(t1.type == ST::Int) return true;
                return (t1_size > t2_size);
            }
            //hopefully unrechable
            return false;
        }
    }

    // *u8 = string literal
    if(t2.type == ST::Char) {
        if(t1.type == ST::u8) {
            if(t1.indirection() == 1) {
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
    if(t.indirection() > 0) {
        t.pindirection -= 1;
    } else {
        return 1;
    }
    return 0;
}

static void increaseDerefTypeInfo(TypeInfo& t) {
    t.pindirection += 1;
    return;
}

FuncDefNode* resolveFunction(FuncCallNode* funccall, SymbolTable* symTab);
static AstNode* getOpFunction(BinOpNode* funccall, SymbolTable* symTab, const TypeInfo& lhst, const TypeInfo& rhst);
static bool isOpOverloadCandidate(const TypeInfo& lhst, const TypeInfo& rhst);

AstNode* currentFunc = nullptr;
std::string currentFunc2;
static void typeCheckPass(AstNode* ast, SymbolTable* symTab) {
    for(auto c : (*(ast->getChildren()))) {
        switch(c->nodeType()) {
            case ANT::FuncDef:
                {
                    auto fundef = static_cast<FuncDefNode*>(c);
                    if(fundef->isTemplated) {
                        break;
                    }
                    //std::cout << __FILE__ << ':' << __FUNCTION__ << " FunctionDef!\n";
                    auto scope = getScope(symTab, fundef->mfuncname + std::to_string(fundef->id));
                    currentFunc = c;
                    typeCheckPass(c,scope);
                    currentFunc = nullptr;
                    //std::cout << "done with function " << ((FuncDefNode*)c)->mfuncname << '\n';
                }
                break;
            case ANT::Block:
                {
                    //std::cout << __FILE__ << ':' << __FUNCTION__ << " Block!\n";
                    //std::cout << "Block ID " << ((BlockNode*)c)->getId() << '\n';
                    auto scope = getScope(symTab, "block"+std::to_string(static_cast<BlockNode*>(c)->getId()));
                    typeCheckPass(c,scope);
                }
                break;
            case ANT::ForLoop:
                {
                    //std::cout << __FILE__ << ':' << __FUNCTION__ << " For!\n";
                    auto scope = getScope(symTab, "for"+std::to_string(static_cast<LoopNode*>(c)->getId()));
                    typeCheckPass(c,scope);
                }
                break;
            case ANT::WhileLoop:
                {
                    //std::cout << __FILE__ << ':' << __FUNCTION__ << " While!\n";
                    auto scope = getScope(symTab, "while"+std::to_string(static_cast<LoopNode*>(c)->getId()));
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
                    typeCheckPass(c,symTab);
                    //std::cout << __FILE__ << ':' << __FUNCTION__ << " FuncCall!\n";
                    auto funccall = static_cast<FuncCallNode*>(c);
                    std::string funcname = funccall->mfuncname;
                    auto fdef = resolveFunction(funccall,symTab);
                    if(!fdef) break;

                    auto funcparams = fdef->getParameters();
                    auto args = funccall->mchildren;
                    std::vector<TypeInfo> arg_types;
                    arg_types.reserve(sizeof(TypeInfo)*args.size());
                    for(auto a : args) {
                        auto tinfo = getTypeInfo(a,symTab);
                        arg_types.push_back(tinfo);
                    }

                    //check every arg, against every parameter
                    for(unsigned int i = 0; i < funcparams.size; i++) {
                        //auto n = args.at(i);
                        //auto param = funcparams.at(i).second;
                        //manual array indexing is sorta iffy... add at() maybe?
                        auto param = funcparams.ptr[i];
                        auto paramt = param->mtypeinfo;
                        auto argt = arg_types.at(i);
                        //check compatibility
                        if(!isSameType(paramt,argt)) {
                            //check for casts
                            if(canCast(paramt,argt)) {
                                CastNode* cast = new CastNode();
                                cast->fromType = argt;
                                cast->mtypeinfo = paramt;
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
                    auto cnode = static_cast<CastNode*>(c);
                    typeCheckPass(c,symTab);
                    TypeInfo t = getTypeInfo(cnode->mchildren.at(0),symTab);
                    cnode->fromType = t;
                }
                break;
            case ANT::UnaryOp:
                {
                    auto binopn = static_cast<BinOpNode*>(c);
                    const char* op = binopn->getOp();
                    typeCheckPass(c,symTab);

                    if(std::strcmp(op,"@") == 0) {
                        TypeInfo t = getTypeInfo(binopn->LHS(),symTab);
                        int err = decreaseDerefTypeInfo(t);
                        if(err) {
                            semanticError(SET::DerefNonPointer,c,symTab);
                        }
                        if(t.type == SemanticType::Void && t.indirection() == 0) {
                            semanticError(SET::VoidPtrDeref,c,symTab);
                        }
                        binopn->mtypeinfo = t;
                    } else if(std::strcmp(op,"-") == 0) {
                        //unary negation
                        TypeInfo lhs_t = getTypeInfo(binopn->LHS(),symTab);
                        if(lhs_t.indirection()) {
                            //TODO(marcus): get actual errors!
                            semanticError(SET::Unknown,binopn,symTab);
                        }
                        //TODO(marcus): actually type check
                        binopn->mtypeinfo = lhs_t;
                    } else if(std::strcmp(op,"&") == 0) {
                        //Address-of op
                        TypeInfo t = getTypeInfo(binopn->LHS(),symTab);
                        increaseDerefTypeInfo(t);
                        binopn->mtypeinfo = t;
                    }
                }
                break;
            case ANT::BinOp:
                {
                    //std::cout << __FILE__ << ':' << __FUNCTION__ << " BinOp!\n";
                    //TODO(marcus): support operator overloading
                    auto binopn = static_cast<BinOpNode*>(c);
                    const char* op = binopn->getOp();

                    typeCheckPass(c,symTab);
                    if(std::strcmp(op,"[") == 0) {
                        //we are indexing into an array, make sure left side is a ptr and right side
                        //is an integer
                        TypeInfo lhst = getTypeInfo(binopn->LHS(),symTab);
                        TypeInfo rhst = getTypeInfo(binopn->RHS(),symTab);
                        int err = decreaseDerefTypeInfo(lhst);
                        if(err) {
                            semanticError(SET::DerefNonPointer,c,symTab);
                        }
                        if(lhst.type == SemanticType::Void && lhst.indirection() == 0) {
                            semanticError(SET::VoidPtrDeref,c,symTab);
                        }
                        binopn->mtypeinfo = lhst;
                    } else if(std::strcmp(op,".") == 0) {
                        //get member name
                        if(binopn->RHS()->nodeType() != ANT::Var) {
                            std::cout << "RHS of . op was not a variable\n";
                            break;
                        }
                        auto lhs_t = getTypeInfo(binopn->LHS(),symTab);
                        std::string structtypename = lhs_t.userid;
                        std::string membername = static_cast<VarNode*>(binopn->RHS())->getVarName();
                        auto tmp = structList.find(structtypename);
                        if(tmp == structList.end()) std::cout << "Error, struct not found...\n";
                        auto strdef = tmp->second;
                        for(auto member : strdef->mchildren) {
                            auto vardec = static_cast<VarDeclNode*>(member);
                            auto var = static_cast<VarNode*>(vardec->mchildren[0]);
                            if(var->getVarName() == membername) {
                                binopn->mtypeinfo = var->mtypeinfo;
                                if(lhs_t.indirection() == 1) {
                                    //TODO(marcus): we need a better way to indicate that the . also
                                    //dereferences
                                    binopn->unaryOp = true;
                                }
                                break;
                            }
                        }
                    } else if(std::strcmp(op,"&") == 0) {
                        //TODO(marcus): implement bitwise-and
                        TypeInfo t = getTypeInfo(binopn->LHS(),symTab);
                        binopn->mtypeinfo = t;
                    } else if(std::strcmp(op,"==") == 0 || std::strcmp(op,"!=") == 0) {
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
                                cast->mtypeinfo = lhs_t;
                                cast->addChild(binopn->RHS());
                                binopn->setRHS(cast);
                            } else if(canCast(rhs_t,lhs_t)) {
                                CastNode* cast = new CastNode();
                                cast->fromType = lhs_t;
                                cast->mtypeinfo = rhs_t;
                                cast->addChild(binopn->LHS());
                                binopn->setLHS(cast);
                            } else {
                                std::cout << "Error with use of operator " << op << '\n';
                                semanticError(SET::MissmatchBinop,binopn,symTab);
                            }
                        }
                    } else {
                        TypeInfo lhs_t = getTypeInfo(binopn->LHS(),symTab);
                        TypeInfo rhs_t = getTypeInfo(binopn->RHS(),symTab);
                        if(isOpOverloadCandidate(lhs_t,rhs_t)) {
                            auto func = getOpFunction(binopn,symTab,lhs_t,rhs_t);
                            binopn->mtypeinfo = func->mtypeinfo;
                            binopn->opOverload = func;
                            break;
                        }

                        //do compatiblity checking
                        if(isSameType(lhs_t,rhs_t)) {
                            binopn->mtypeinfo = lhs_t;
                            //binopn->mstype = lhs_t.type;
                        } else {
                            //check for pointer math
                            if(isPointerMath(lhs_t,rhs_t)) {
                                if(lhs_t.indirection()) {
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
                                cast->mtypeinfo = lhs_t;
                                cast->addChild(binopn->RHS());
                                binopn->setRHS(cast);
                            } else if(canCast(rhs_t,lhs_t)) {
                                binopn->mtypeinfo = rhs_t;
                                CastNode* cast = new CastNode();
                                cast->fromType = lhs_t;
                                cast->mtypeinfo = rhs_t;
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
                    auto retn = static_cast<ReturnNode*>(c);
                    TypeInfo ret_typeinfo;
                    ret_typeinfo.type = SemanticType::Void;
                    AstNode* expr = nullptr;
                    if(retn->mchildren.size() != 0) {
                        expr = retn->mchildren.at(0);
                        typeCheckPass(c,symTab);
                        ret_typeinfo = getTypeInfo(expr,symTab);
                    }
                    std::string name = static_cast<FuncDefNode*>(currentFunc)->mfuncname;
                    name += std::to_string(static_cast<FuncDefNode*>(currentFunc)->id);
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
                            cast->mtypeinfo = func_typeinfo;
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
                    auto assignn = static_cast<AssignNode*>(c);
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
                            cast->mtypeinfo = lhs_typeinfo;
                            cast->addChild(rhs);
                            assignn->mchildren[1] = cast;
                        } else {
                            semanticError(SET::MissmatchAssign,c,symTab);
                        }
                    }

                }
                break;
            case ANT::VarDec:
                {
                    auto vdeca = static_cast<VarDeclNode*>(c);
                    auto lhs = vdeca->getLHS();
                    TypeInfo lhs_typeinfo = getTypeInfo(lhs, symTab);

                    //Make sure struct is defined
                    if(lhs_typeinfo.type == SemanticType::User) {
                        std::string structtypename = lhs_typeinfo.userid;
                        auto tmp = structList.find(structtypename);
                        if(tmp == structList.end()) semanticError(SET::StructNotDefined,c,symTab);
                    }
                }
                break;
            case ANT::VarDecAssign:
                {
                    //std::cout << __FILE__ << ':' << __FUNCTION__ << " VarDecAssign!\n";
                    auto vdeca = static_cast<VarDeclNode*>(c);
                    auto lhs = vdeca->getLHS();
                    auto rhs = vdeca->getRHS();
                    typeCheckPass(c, symTab);
                    TypeInfo lhs_typeinfo = getTypeInfo(c, symTab);
                    TypeInfo rhs_typeinfo = getTypeInfo(rhs,symTab);

                    //Make sure struct is defined
                    if(lhs_typeinfo.type == SemanticType::User) {
                        std::string structtypename = lhs_typeinfo.userid;
                        auto tmp = structList.find(structtypename);
                        if(tmp == structList.end()) semanticError(SET::StructNotDefined,c,symTab);
                    }

                    if(lhs_typeinfo.type == SemanticType::Infer) {
                        //Infer *void from null
                        if(rhs_typeinfo.type == SemanticType::nulllit) {
                            rhs_typeinfo.type = SemanticType::Void;
                            rhs_typeinfo.pindirection = 1;
                        }
                        //std::cout << "Inferred type!\n";
                        std::string varname = static_cast<VarNode*>(lhs)->getVarName();
                        //std::cout << "Inferring type for var " << varname << '\n';
                        //std::cout << rhs_typeinfo << '\n';
                        updateVarEntry(symTab,rhs_typeinfo,std::move(varname));
                        c->mtypeinfo = rhs_typeinfo;
                        break;
                    }


                    //Do compatibility checking
                    if(!isSameType(lhs_typeinfo,rhs_typeinfo)) {
                    //check for casts
                        if(canCast(lhs_typeinfo,rhs_typeinfo)) {
                            //assignn->mtypeinfo = lhs_typeinfo;
                            CastNode* cast = new CastNode();
                            cast->fromType = rhs_typeinfo;
                            cast->mtypeinfo = lhs_typeinfo;
                            cast->addChild(rhs);
                            vdeca->mchildren[1] = cast;
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
                    auto scope = getScope(symTab, static_cast<FuncDefNode*>(c)->mfuncname);
                    typeCheckPass(c,scope);
                }
                break;
                /*
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
                */
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
                return static_cast<ConstantNode*>(ast)->mtypeinfo;
            }
            break;
        case ANT::Var:
            {
                std::string name = static_cast<VarNode*>(ast)->getVarName();
                auto entries = getEntry(symTab,name);
                if(entries.size() > 0) {
                    SymbolTableEntry* e = entries.at(0);
                    ast->mtypeinfo = e->typeinfo;
                    return e->typeinfo;
                } else {
                    //TODO(marcus): We need some way of determining if
                    //we are accessing a struct var or not.
                    std::cout << "No Entries! Var name is " << name << "\n";
                }
            }
            break;
        case ANT::VarDec:
            {
                std::string name = static_cast<VarNode*>(static_cast<VarDeclNode*>(ast)->getLHS())->getVarName();
                auto entries = getEntry(symTab,std::move(name));
                SymbolTableEntry* e = entries.at(0);
                return e->typeinfo;
            }
            break;
        case ANT::VarDecAssign:
            {
                std::string name = static_cast<VarNode*>(static_cast<VarDeclNode*>(ast)->getLHS())->getVarName();
                auto entries = getEntry(symTab,std::move(name));
                SymbolTableEntry* e = entries.at(0);
                return e->typeinfo;
            }
            break;
        case ANT::FuncCall:
            {
                //TODO(marcus): will have to deal with overloads somehow
                std::string name = static_cast<FuncCallNode*>(ast)->mfuncname;
                auto entries = getEntry(symTab,name);
                if(entries.size() == 0) {
                    entries = getEntry(symTab,name,static_cast<FuncCallNode*>(ast)->scopes);
                }
                SymbolTableEntry* e = entries.at(0);
                return e->node->mtypeinfo;
            }
            break;
        case ANT::UnaryOp:
        case ANT::BinOp:
            {
                auto binopn = static_cast<BinOpNode*>(ast);
                return binopn->mtypeinfo;
            }
        case ANT::Cast:
            {
                auto castnode = static_cast<CastNode*>(ast);
                return castnode->mtypeinfo;
            }
        default:
            break;
    }
    //TODO(marcus): add an error type
    TypeInfo error;
    return error;
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
        } else if(nodetype == AstNodeType::LoopStmtCnt || nodetype == AstNodeType::LoopStmtBrk) {
            //end of loop scope, iterate through all defer statements in last stack
            if(deferStacks.size() > 0) {
                auto defers = deferStacks.back();
                for(auto deferstmt = defers.rbegin(); deferstmt != defers.rend(); deferstmt++) {
                    auto defer_node = static_cast<DeferStmtNode*>(*deferstmt);
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
                    auto defer_node = static_cast<DeferStmtNode*>(*deferstmt);
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
                auto defer_node = static_cast<DeferStmtNode*>(*deferstmt);
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

static std::pair<int,int> typeSizeAndAlign(const TypeInfo& t) {
    int size = 0;
    int align = 0;
    //TODO(marcus): Support array sizes
    if(t.indirection()) {
        //NOTE(marcus): this is platform dependent
        size = align = 8;
    } else {
        switch(t.type) {
            case ST::Bool:
            case ST::Char:
            case ST::u8:
            case ST::s8:
                size = align = 1;
                break;
            case ST::u16:
            case ST::s16:
                size = align = 2;
                break;
            case ST::u32:
            case ST::s32:
            case ST::Float:
            case ST::Int:
            case ST::intlit:
            case ST::floatlit:
                size = align = 4;
                break;
            case ST::u64:
            case ST::s64:
            case ST::Double:
                size = align = 8;
                break;
            case ST::User:
                {
                    //stargate
                    auto iter = structList.find(std::string(t.userid));
                    if(iter == structList.end()) {
                        //TODO(marcus): error, struct type doesn't exist
                        semanticError(SemanticErrorType::Unknown, nullptr, &progSymTab);
                    }
                    auto struct_node = static_cast<StructDefNode*>(iter->second);

                    int maximum_size = 0;
                    int maximum_align = 0;
                    int struct_size = 0;
                    for(auto member : struct_node->mchildren) {
                        if(member->nodeType() != AstNodeType::VarDec) continue;
                        auto vardec = static_cast<VarDeclNode*>(member);
                        auto var = static_cast<VarNode*>(vardec->mchildren[0]);
                        auto member_t = var->mtypeinfo;
                        auto m_size_align = typeSizeAndAlign(member_t);
                        maximum_size = std::max(maximum_size,m_size_align.first);
                        maximum_align = std::max(maximum_align,m_size_align.second);
                        int padding = (m_size_align.second - (struct_size % m_size_align.second)) % m_size_align.second;
                        struct_size += m_size_align.first + padding;
                    }
                    //Empty structs are size 1
                    if(struct_size == 0) {
                        struct_size = 1;
                        maximum_align = 1;
                    }
                    if(struct_node->nodeType() == AstNodeType::UnionDef) {
                        //unions are the size of the largest member
                        struct_size = maximum_size;
                    }
                    struct_size += (maximum_align - (struct_size % maximum_align)) % maximum_align;
                    size = struct_size;
                    align = maximum_align;
                }
                break;
            default:
                //TODO(marcus): fix this error
                semanticError(SemanticErrorType::Unknown, nullptr, &progSymTab);
                break;
        }
    }
    return std::pair<int,int>(size,align);
}

static int calcTypeSize(const TypeInfo& t) {
    auto ret = typeSizeAndAlign(t);
    return ret.first;
}

#undef ST

void resolveSizeOfs(AstNode* ast) {
    std::vector<AstNode*>* vec = ast->getChildren();
    for(unsigned int i = 0; i < vec->size(); i++) {
        AstNode* child = (*vec)[i];
        if(child->nodeType() == ANT::SizeOf) {
            int size = calcTypeSize(child->mtypeinfo);
            char* buff = (char*)malloc(12);
            int wrote = sprintf(buff,"%d",size);
            buff[11] = '\0';
            assert(wrote <= 10);
            const char* val = buff;
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

int callSiteMatchesFunc(FuncDefNode* fn, FuncCallNode* call, SymbolTable* symTab) {
    auto funcparams = fn->getParameters();
    if(funcparams.size != call->mchildren.size()) {
        return -1;
    }
    bool matched = true;

    //check every arg, parameter pair
    for(unsigned int i = 0; i < funcparams.size; i++) {
        auto param = funcparams.ptr[i];
        auto paramt = param->mtypeinfo;
        auto argn = call->mchildren.at(i);
        auto argt = getTypeInfo(argn,symTab);

        if(!isSameType(paramt,argt)) {
            if(!canCast(paramt,argt)) {
                //std::cout << "types different " << paramt << " and " << argt << '\n';
                matched = false;
                break;
            }
        }
    }
    if(matched) return 1;
    return 0;
}

FuncDefNode* getTemplatedFunc(FuncCallNode* call) {
    auto funcname = call->mfuncname;
    for(auto ptr : templateList) {
        auto fdef = static_cast<FuncDefNode*>(ptr);
        if(fdef->mfuncname == funcname) {
            if(fdef->mchildren.size()-1 == call->mchildren.size())
                return fdef;
        }
    }
    return nullptr;
}



FuncDefNode* resolveFunction(FuncCallNode* funccall, SymbolTable* symTab) {
    auto funcname = funccall->mfuncname;

    //If we already know the function call is for a template, just instantiate it
    if(funccall->specialized) {
        auto templateFunc = getTemplatedFunc(funccall);
        if(!templateFunc) {
            semanticError(SemanticErrorType::NoFunction, funccall, symTab);
            return nullptr;
        } else {
            //std::cout << "Call is known to be a template\n";
            //FuncCall matches to a template, need to instantiate
            auto save_currentFunc = currentFunc;
            auto instanced_func = static_cast<FuncDefNode*>(instantiateTemplate(funccall,templateFunc,symTab));
            currentFunc = save_currentFunc;
            //Finished instantiating, get entries
            int matches = callSiteMatchesFunc(instanced_func,funccall,symTab);
            if(matches > 0) {
                funccall->func = instanced_func;
                return instanced_func;
            }
            semanticError(SemanticErrorType::NoResolve, funccall, symTab);
            return nullptr;
        }
    }
    auto entries = getEntry(symTab,funcname);
    FuncDefNode* templateFunc = nullptr;

    if(entries.size() == 0) {
        entries = getEntry(symTab,funcname,funccall->scopes);
        if(entries.size() == 0) {
            templateFunc = getTemplatedFunc(funccall);
            if(!templateFunc) {
                semanticError(SemanticErrorType::NoFunction, funccall, symTab);
                return nullptr;
            } else {
                //FuncCall matches to a template, need to instantiate
                auto save_currentFunc = currentFunc;
                auto instanced_func = instantiateTemplate(funccall,templateFunc,symTab);
                currentFunc = save_currentFunc;
                //Finished instantiating, get entries
                entries = getEntry(symTab,funcname);
            }
        }
    }
    auto e = entries.at(0);

    //TODO(marcus): Shouold we allow mix of prototypes and mangled funcs of same name?
    //eg extern foo() and foo()
    if(e->node->nodeType() == AstNodeType::Prototype) {
        int matches = callSiteMatchesFunc(static_cast<FuncDefNode*>(e->node),funccall,symTab);
        if(matches == -1) {
            auto funcparams = static_cast<FuncDefNode*>(e->node)->getParameters();
            std::cout << "Missmatched number of parameters. At Callsite: " << funccall->mchildren.size() << " Function: " << funcparams.size << '\n';
            return nullptr;
        } else if(matches == 0) {
            semanticError(SemanticErrorType::NoResolve, funccall, symTab);
            return nullptr;
        }
        return static_cast<FuncDefNode*>(e->node);
    } else {

        //TODO(marcus): this matches the first overload found, not the best one.
        FuncDefNode* matchedNode = nullptr;
        for(auto overload : e->overloads) {
            FuncDefNode* candidate = nullptr;
            if(overload->nodeType() == AstNodeType::FuncDef) {
                candidate = static_cast<FuncDefNode*>(overload);
            }
            int matches = callSiteMatchesFunc(candidate,funccall,symTab);
            if(matches == -1) {
                continue;
            } else if(matches > 0) {
                if(!matchedNode) {
                    matchedNode = candidate;
                } else {
                    semanticError(SemanticErrorType::MultipleFuncResolve, funccall, symTab);
                    //std::cout << "Error. Multiple matching functions found.\n";
                }
            }
        }

        if(matchedNode) {
            auto mangled = matchedNode->mangledName();
            //std::cout << "Matched: " << mangled << '\n';
            //std::cout << "Replacing function call name\n";
            funccall->func = matchedNode;
        } else {
            //No matching function found, see if we can instantiate a template
            templateFunc = getTemplatedFunc(funccall);
            if(!templateFunc) {
                semanticError(SemanticErrorType::NoResolve, funccall, symTab);
                return nullptr;
            } else {
                //FuncCall matches to a template, need to instantiate
                auto save_currentFunc = currentFunc;
                auto instanced_func = static_cast<FuncDefNode*>(instantiateTemplate(funccall,templateFunc,symTab));
                currentFunc = save_currentFunc;
                //Finished instantiating, get entries
                //entries = getEntry(symTab,funcname);
                //auto e = entries.at(0);
                int matches = callSiteMatchesFunc(instanced_func,funccall,symTab);
                if(matches > 0) {
                    funccall->func = instanced_func;
                    return instanced_func;
                }
                semanticError(SemanticErrorType::NoResolve, funccall, symTab);
                return nullptr;
            }
            //semanticError(SemanticErrorType::NoResolve, funccall, symTab);
        }
        //return (matchedNode != nullptr);
        return matchedNode;
    }
}

static bool isOpOverloadCandidate(const TypeInfo& lhst, const TypeInfo& rhst) {
    //If either the left or the right side of the operator is a user type we need to look up
    //an operator overload
    bool lhs_is_user = ((lhst.type == SemanticType::User) && (lhst.indirection() == 0));
    bool rhs_is_user = ((rhst.type == SemanticType::User) && (rhst.indirection() == 0));
    return (lhs_is_user || rhs_is_user);
}

static void importPrepass(AstNode* ast, SymbolTable* symTab) {
    auto compileunit = static_cast<CompileUnitNode*>(ast);
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
        if(c->nodeType() == ANT::CompileUnit) {
            auto scope = getScope(&progSymTab, static_cast<CompileUnitNode*>(c)->getFileName());
            importPrepass(c,scope);
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
                    auto lhs = static_cast<AssignNode*>(c)->getLHS();
                    auto rhs = static_cast<AssignNode*>(c)->getRHS();

                    if(t == TokenType::assignment)
                        break;

                    auto bop = new BinOpNode(AstNodeType::BinOp);
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

static const char* getOperatorName(const char* op)
{
    if(std::strcmp(op,"+") == 0) {
        return "plus";
    } else if(std::strcmp(op,"-") == 0) {
        return "minus";
    } else if(std::strcmp(op,"*") == 0) {
        return "mult";
    } else if(std::strcmp(op,"/") == 0) {
        return "div";
    }
    return "noop";
}

static AstNode* getOpFunction(BinOpNode* funccall, SymbolTable* symTab, const TypeInfo& lhst, const TypeInfo& rhst)
{
    //TODO(marcus): searches for the proper op overload for a given node.
    //TODO(marcus): op overloads are global right now, maybe should be in symbol table?
    auto op = funccall->getOp();
    std::string opstr = getOperatorName(op);
    opstr = "op_"+opstr;
    std::cout << "Searching for op: " << opstr << '\n';
    auto res = operatorOverloads.find(opstr);

    //TODO(marcus): we have a user type with no operator overloads for this operator, error out
    if(res == operatorOverloads.end()) {
        return nullptr;
    }
    //NOTE(marcus): we are copying the vector here, may want to make this better later.
    auto overloads = res->second;
    AstNode* bestMatch = nullptr;
    TypeInfo arg_typeinfo[] = {lhst, rhst};
    //check overloads for a matching function
    for(auto func : overloads) {
        FuncDefNode* candidate = func;
        auto funcparams = candidate->getParameters();

        bool matched = true;

        //check every arg, against every parameter
        for(unsigned int i = 0; i < funcparams.size; i++) {
            auto param = funcparams.ptr[i];
            auto paramt = param->mtypeinfo;
            auto argt = arg_typeinfo[i];

            if(!isSameType(paramt,argt)) {
                if(!canCast(paramt,argt)) {
                    matched = false;
                    break;
                }
            }
        }
        if(matched) {
            if(!bestMatch) {
                bestMatch = candidate;
            } else {
                //TODO(marcus): this error expects a FuncCallNode, not a binopnode
                semanticError(SemanticErrorType::MultipleFuncResolve, funccall, symTab);
            }
        }
    }
    return bestMatch;
}

static void checkForRecursiveTypes() {
    std::set<StructDefNode*> valid_types;

    //Validate all structs/unions made of only primitive types
    for(auto iter : structList) {
        auto n = static_cast<StructDefNode*>(iter.second);
        bool is_simple = true;
        for(auto c : (*(n->getChildren()))) {
            if(c->nodeType() == AstNodeType::VarDec) {
                if((c->mtypeinfo.type == SemanticType::User) && (c->mtypeinfo.indirection() == 0)) {
                    is_simple = false;
                    break;
                }
            }
        }
        if(is_simple) {
            valid_types.insert(n);
        }
    }

    //Validate any structs where all the struct members are validated
    bool did_change;
    do {
        did_change = false;
        bool finished = true;
        for(auto iter : structList) {
            auto n = static_cast<StructDefNode*>(iter.second);
            //skip structs we already validated
            if(valid_types.find(n) != valid_types.end()) {
                continue;
            }
            //we have a struct we haven't validated, we aren't finished
            finished = false;

            //have we validated out every member type?
            bool all_members_valid = true;
            for(auto c : (*(n->getChildren()))) {
                if(c->nodeType() == AstNodeType::VarDec) {
                    if((c->mtypeinfo.type == SemanticType::User) && (c->mtypeinfo.indirection() == 0)) {
                        auto type_str = c->mtypeinfo.userid;
                        bool struct_is_valid = false;
                        for(auto s : valid_types) {
                            if(std::strcmp(s->getIdent().c_str(),type_str) == 0) {
                                struct_is_valid = true;
                                break;
                            }
                        }
                        if(!struct_is_valid) {
                            //haven't validated the types for every member of this struct
                            all_members_valid = false;
                            break;
                        }
                    }
                }
            }

            if(all_members_valid) {
                did_change = true;
                valid_types.insert(n);
                break;
            }
        }
        if(finished) {
            did_change = true;
            break;
        }
    } while(did_change);
    if(!did_change) {
        std::vector<StructDefNode*> unresolvedTypes;
        for(auto iter : structList) {
            auto n = static_cast<StructDefNode*>(iter.second);
            if(valid_types.find(n) == valid_types.end()) {
                unresolvedTypes.push_back(n);
            }
        }
        semanticError(SET::CyclicTypeDefinitions,unresolvedTypes);
    }
}

static void cloneTree(AstNode* parent, AstNode* node, std::unordered_map<std::string,TypeInfo>& typeMap) {
    AstNode* cloned = nullptr;
    switch(node->nodeType()) {
        case AstNodeType::Params:
            {
                auto t_param = static_cast<ParamsNode*>(node);
                auto param = new ParamsNode(t_param);
                cloned = param;
                if(param->mtypeinfo.type == SemanticType::Template) {
                    auto replaceType = typeMap[param->mtypeinfo.userid];
                    replaceType.pindirection = param->mtypeinfo.pindirection;
                    param->mtypeinfo = replaceType;
                }
            }
            break;
        case AstNodeType::Block:
            {
                auto bn = new BlockNode();
                cloned = bn;
            }
            break;
        case AstNodeType::Var:
            {
                auto t_vn = static_cast<VarNode*>(node);
                auto vn = new VarNode(t_vn);
                cloned = vn;
                if(vn->mtypeinfo.type == SemanticType::Template) {
                    int pindr = vn->mtypeinfo.pindirection;
                    vn->mtypeinfo = typeMap[vn->mtypeinfo.userid];
                    vn->mtypeinfo.pindirection = pindr;
                }
            }
            break;
        case AstNodeType::VarDec:
        case AstNodeType::VarDecAssign:
            {
                auto t_vdn = static_cast<VarDeclNode*>(node);
                auto vdn = new VarDeclNode(t_vdn);
                cloned = vdn;
                if(vdn->mtypeinfo.type == SemanticType::Template) {
                    int pindr = vdn->mtypeinfo.pindirection;
                    vdn->mtypeinfo = typeMap[vdn->mtypeinfo.userid];
                    vdn->mtypeinfo.pindirection = pindr;
                }
            }
            break;
        case AstNodeType::RetStmnt:
            {
                auto t_ret = static_cast<ReturnNode*>(node);
                auto ret = new ReturnNode(t_ret);
                cloned = ret;
            }
            break;
        case AstNodeType::UnaryOp:
        case AstNodeType::BinOp:
            {
                auto t_bop = static_cast<BinOpNode*>(node);
                auto bop = new BinOpNode(t_bop);
                cloned = bop;
            }
            break;
        case AstNodeType::IfStmt:
            {
                auto t_if = static_cast<IfNode*>(node);
                auto ifn = new IfNode(t_if);
                cloned = ifn;
            }
            break;
        case AstNodeType::WhileLoop:
        case AstNodeType::ForLoop:
            {
                auto t_loop = static_cast<LoopNode*>(node);
                auto loopn = new LoopNode(t_loop);
                cloned = loopn;
            }
            break;
        case AstNodeType::LoopStmtBrk:
        case AstNodeType::LoopStmtCnt:
            {
                auto t_loops = static_cast<LoopStmtNode*>(node);
                auto loops = new LoopStmtNode(t_loops);
                cloned = loops;
            }
            break;
        case AstNodeType::Cast:
            {
                auto t_cast = static_cast<CastNode*>(node);
                auto cast = new CastNode(t_cast);
                cloned = cast;
                if(cast->mtypeinfo.type == SemanticType::Template) {
                    int pindr = cast->mtypeinfo.pindirection;
                    cast->mtypeinfo = typeMap[cast->mtypeinfo.userid];
                    cast->mtypeinfo.pindirection = pindr;
                }
                if(cast->fromType.type == SemanticType::Template) {
                    int pindr = cast->fromType.pindirection;
                    cast->fromType = typeMap[cast->fromType.userid];
                    cast->fromType.pindirection = pindr;
                }
            }
            break;
        case AstNodeType::Assign:
            {
                auto t_asgn = static_cast<AssignNode*>(node);
                auto asgn = new AssignNode(t_asgn);
                cloned = asgn;
            }
            break;
        case AstNodeType::Const:
            {
                auto t_const = static_cast<ConstantNode*>(node);
                auto constn = new ConstantNode(t_const);
                cloned = constn;
            }
            break;
        case AstNodeType::DeferStmt:
            {
                auto dfr = new DeferStmtNode();
                cloned = dfr;
            }
            break;
        case AstNodeType::SizeOf:
            {
                auto sof = new SizeOfNode();
                cloned = sof;
                sof->mtypeinfo = node->mtypeinfo;
                if(sof->mtypeinfo.type == SemanticType::Template) {
                    int pindr = sof->mtypeinfo.pindirection;
                    sof->mtypeinfo = typeMap[sof->mtypeinfo.userid];
                    sof->mtypeinfo.pindirection = pindr;
                }
            }
            break;
        case AstNodeType::FuncCall:
            {
                auto t_fcall = static_cast<FuncCallNode*>(node);
                auto fcall = new FuncCallNode(t_fcall);
                cloned = fcall;
                if(fcall->specialized) {
                    //map of template varname to type for current function
                    //map of template varname to possible templated type for call
                    for(auto mapping : fcall->templateTypeParameters) {
                        if(mapping.second.type == SemanticType::Template) {
                            if(typeMap.find(mapping.second.userid) != typeMap.end()) {
                                //std::cout << "Replacing templated type\n";
                                //std::cout << "New type is " << typeMap[mapping.second.userid] << '\n';
                                fcall->templateTypeParameters[mapping.first] = typeMap[mapping.second.userid];
                            }
                        }
                    }
                }
            }
            break;
        default:
            break;
    }
    parent->addChild(cloned);
    for(auto c : node->mchildren) {
        cloneTree(cloned,c,typeMap);
    }
    return;
}

static AstNode* cloneTree(AstNode* t_func, std::unordered_map<std::string,TypeInfo>& typeMap) {
    auto t_fdef = static_cast<FuncDefNode*>(t_func);
    auto fdef = new FuncDefNode(t_fdef);
    if(fdef->mtypeinfo.type == SemanticType::Template) {
        int pindr = fdef->mtypeinfo.pindirection;
        fdef->mtypeinfo = typeMap[fdef->mtypeinfo.userid];
        fdef->mtypeinfo.pindirection = pindr;
        //std::cout << "Return type is now... " << fdef->mtypeinfo << '\n';
    }
    for(auto c : t_func->mchildren) {
        cloneTree(fdef,c,typeMap);
    }
    return fdef;
}

static AstNode* instantiateTemplate(FuncCallNode* funccall, FuncDefNode* funcdef, SymbolTable* symTab)
{
    auto func_args = funccall->mchildren;
    auto func_params = funcdef->getParameters();
    std::unordered_map<std::string,TypeInfo> typeMap;

    if(funccall->specialized) {
        //std::cout << "Template is specialized!\n";
        //type mappings given by the user
        typeMap = funccall->templateTypeParameters;
        //TODO(marcus): warn/error on unused mappings within the template?
    } else {
        //type mappings are inferred
        //Make a map of templated types to real types
        int idx = 0;
        for(auto p : func_params) {
            auto pt = p->mtypeinfo;
            if(pt.type == SemanticType::Template) {
                auto argn = func_args[idx];
                auto argt = getTypeInfo(argn,symTab);
                if(typeMap.find(pt.userid) == typeMap.end()) {
                    typeMap[pt.userid] = argt;
                } else {
                    //TODO(marcus): Check that types are castable?
                }
            }
            idx++;
        }
    }
    //Check for matching concrete function
    auto funcname = funccall->mfuncname;
    for(auto fn : instantiatedFunctionsList) {
        if(fn->mfuncname !=  funcname) {
            continue;
        }
        //instatiated functions match
        int matches = callSiteMatchesFunc(fn,funccall,symTab);
        if(matches == 0) continue;
        else if(matches > 0) {
            //std::cout << "Matched Concrete Specialization found\n";
            return fn;
        }
    }
    //std::cout << "Instantiating function " << funcname << '\n';

    //clone tree, replacing types as needed
    auto instancedFunc = cloneTree(funcdef,typeMap);
    instantiatedFunctionsList.push_back(static_cast<FuncDefNode*>(instancedFunc));

    //run semantic passes on new tree
    auto fileScope = getFileScope(symTab);
    AstNode tmp;
    tmp.addChild(instancedFunc);

    //std::cout << "Got file scope " << fileScope->name << "\n";

    //TODO(marcus): it is odd that some passes need a fake parent to work
    transformAssignments(instancedFunc);
    //populateTypeList(ast); //does nothing, no type defs in functions allowed
    resolveSizeOfs(instancedFunc);
    //std::cout << "Populating SymTab With Function\n";
    populateSymbolTableFunctions(&tmp,fileScope);
    semanticPass1(instancedFunc,0,fileScope);
    typeCheckPass(&tmp, fileScope);
    deferPass(&tmp);

    //std::cout << "Finished, " << funcname << " is now a concrete function\n";

    return instancedFunc;
}

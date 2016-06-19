#include "irgen.h"
#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <iostream>
#include "astnode.h"
#include "astnodetypes.h"

using namespace llvm;

LLVMContext context;
IRBuilder<> Builder(context);
Module* module;

#define ST SemanticType
Type* getIRType(ST t, std::string ident = "") {
    Type* ret;
    switch(t) {
        case ST::Void:
            ret = Type::getVoidTy(context);
            break;
        case ST::Int:
            ret = Type::getInt32Ty(context);
            break;
        case ST::Float:
            ret = Type::getFloatTy(context);
            break;
        case ST::Double:
            ret = Type::getDoubleTy(context);
            break;
        case ST::Char:
            ret = Type::getInt8Ty(context);
            break;
        case ST::Bool:
            ret = Type::getInt1Ty(context);
            break;
        default:
            std::cout << "Type not supported, defaulting to void\n";
            ret = Type::getVoidTy(context);
            break;
    }

    return ret;
}
#undef ST


Function* prototypeCodegen(AstNode* n) {
    PrototypeNode* protonode = (PrototypeNode*) n;
    std::vector<AstNode*>* vec = protonode->getParameters();
    std::vector<Type*> parameterTypes;
    parameterTypes.reserve(vec->size());
   

    //TODO(marcus): handle user types for return/parameters
    for(auto c : (*vec)) {
        Type* t = getIRType(c->getType());
        parameterTypes.push_back(t);
    }
    
    Type* retType = getIRType(protonode->getType());
    
    FunctionType* FT = FunctionType::get(retType, parameterTypes, false);
    Function* F = Function::Create(FT, Function::ExternalLinkage, protonode->mfuncname, module);

    unsigned int idx = 0;
    for(auto &Arg : F->args()) {
        std::string name = ((ParamsNode*) (*vec)[idx])->mname;
        Arg.setName(name);
        ++idx;
    }
    delete vec;
    return F;
}

#define ANT AstNodeType
void generateIR_llvm(AstNode* ast) {
    
    //check for null
    if(!ast)
        return;

    //Handle IR gen for each node type
    switch(ast->nodeType()) {
        case ANT::Prototype:
            {
            Function* f = prototypeCodegen(ast);
            //f->dump();
            }
            break;
        default:
            break;
    }

    //recurse
    std::vector<AstNode*>* vec = ast->getChildren();
    for(auto c : (*vec)) {
        generateIR_llvm(c);
    }
}

void dumpIR() {
    module->dump();
    return;
}

void generateIR(AstNode* ast) {
    module = new Module("Neuro Program", context);
    generateIR_llvm(ast);
}

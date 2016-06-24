#include "irgen.h"
#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/BasicBlock.h>
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

Function* functionCodgen(AstNode* n) {
    FuncDefNode* funcnode = (FuncDefNode*) n;
    Function* F = module->getFunction(funcnode->mfuncname);
    if(!F) {
        std::vector<AstNode*>* vec = funcnode->getParameters();
        std::vector<Type*> parameterTypes;
        parameterTypes.reserve(vec->size());
       

        //TODO(marcus): handle user types for return/parameters
        for(auto c : (*vec)) {
            Type* t = getIRType(c->getType());
            parameterTypes.push_back(t);
        }
        
        Type* retType = getIRType(funcnode->getType());
        
        FunctionType* FT = FunctionType::get(retType, parameterTypes, false);
        F = Function::Create(FT, Function::ExternalLinkage, funcnode->mfuncname, module);

        unsigned int idx = 0;
        for(auto &Arg : F->args()) {
            std::string name = ((ParamsNode*) (*vec)[idx])->mname;
            Arg.setName(name);
            ++idx;
        }
        delete vec;
    }

    //TODO(marcus): what is entry, can it be used for every function?
    BasicBlock* BB = BasicBlock::Create(context, "entry", F);
    Builder.SetInsertPoint(BB);
    //FIXME(marcus): remove void return once expression codegen works
    Builder.CreateRetVoid();
    return F;
}

Value* funcCallCodegen(AstNode* n) {
    FuncCallNode* callnode = (FuncCallNode*) n;
    Function* F = module->getFunction(callnode->mfuncname);
    if(!F) {
        std::cout << "Function lookup not found!\n";
        return nullptr;
    }

    std::vector<Value*> args;
    std::vector<AstNode*>* vec = callnode->getChildren();
    for(auto c : (*vec)) {
        //TODO(marcus): generate code properly for expressions
        args.push_back(ConstantInt::get(context, APInt()));
    }

    return Builder.CreateCall(F, args, "calltemp");
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
                return;
            }
            break;
        case ANT::FuncDef:
            {
                Function* F = functionCodgen(ast);
                //TODO(marcus): codegen function body
            }
            break;
        case ANT::FuncCall:
            {
                std::cout << "generating function call\n";
                funcCallCodegen(ast);
                return;
            }
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

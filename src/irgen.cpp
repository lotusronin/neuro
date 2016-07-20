#include "irgen.h"
#include "llvmirgen.h"
#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/BasicBlock.h>
#include <iostream>
#include <string>
#include "astnode.h"
#include "astnodetypes.h"

using namespace llvm;

LLVMContext context;
IRBuilder<> Builder(context);
Module* module;

//TODO(marcus): make this a tree of mapped values.
std::map<std::string, AllocaInst*> varTable;

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
    //Builder.CreateRetVoid();
    return F;
}

Value* funcCallCodegen(AstNode* n) {
    FuncCallNode* callnode = (FuncCallNode*) n;
    Function* F = module->getFunction(callnode->mfuncname);
    if(!F) {
        std::cout << "Function lookup for " << callnode->mfuncname << " not found!\n";
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

Value* retCodegen(AstNode* n) {
    ReturnNode* retnode = (ReturnNode*) n;
    auto pchildren = retnode->getChildren();
    Value* ret = nullptr;
    if(pchildren->size() > 0) {
        Value* retval = expressionCodegen((*retnode->getChildren())[0]);
        ret = Builder.CreateRet(retval);
    } else {
        ret = Builder.CreateRetVoid();
    }
    return ret;
}

Value* expressionCodegen(AstNode* n) {
    //TODO(marcus): actually fill this in.
    Value* val = ConstantInt::get(context, APInt(32,0));
    return val;
}

void blockCodegen(AstNode* n) {
    std::vector<AstNode*>* vec = n->getChildren();
    for(auto c : (*vec)) {
        statementCodegen(c);
    }
}

//TODO(marcus): see if this code needs to be merged into one function later
void vardecCodegen(AstNode* n) {
    auto vardecn = (VarDecNode*) n;
    auto varn = (VarNode*)vardecn->mchildren.at(0);
    //FIXME(marcus): get the type of the node once type checking works
    //TODO(marcus): fix how you access the name of the variable
    auto alloc = Builder.CreateAlloca(Type::getInt32Ty(context),0,varn->getVarName());
    varTable[varn->getVarName()] = alloc;
    return;
}

void vardecassignCodegen(AstNode* n) {
    auto vardecan = (VarDecAssignNode*) n;
    auto varn = (VarNode*)vardecan->mchildren.at(0);
    //FIXME(marcus): get the type of the node once type checking works
    //TODO(marcus): fix how you access the name of the variable
    AllocaInst* alloca = Builder.CreateAlloca(Type::getInt32Ty(context),0,varn->getVarName());
    varTable[varn->getVarName()] = alloca;
    //TODO(marcus): don't hardcode child accesses
    Value* val = expressionCodegen(vardecan->mchildren.at(1));
    Builder.CreateStore(val,alloca);
    return;
}

void assignCodegen(AstNode* n) {
    std::cout << "Generating assingment\n";
    //TODO(marcus): don't hardcode child access
    auto assignn = (AssignNode*)n;
    auto varn = (VarNode*) assignn->mchildren.at(0);
    auto rhs = assignn->mchildren.at(1);
    //TODO(marcus): make left hand side work for expressions that yield an address
    Value* var = varTable[varn->getVarName()];
    Value* val = expressionCodegen(rhs);
    Builder.CreateStore(val,var);
    return;
}

#define ANT AstNodeType
void statementCodegen(AstNode* n) {
    switch(n->nodeType()) {
        case ANT::RetStmnt:
                retCodegen(n);
                break;
        case ANT::Block:
                blockCodegen(n);
                break;
        case ANT::VarDec:
                vardecCodegen(n);
                break;
        case ANT::VarDecAssign:
                vardecassignCodegen(n);
                break;
        case ANT::Assign:
                assignCodegen(n);
                break;
        default:
            break;
    }
}

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
               statementCodegen(((FuncDefNode*)ast)->getFunctionBody());
               return;
            }
            break;
        case ANT::FuncCall:
            {
                std::cout << "generating function call\n";
                funcCallCodegen(ast);
                return;
            }
        case ANT::RetStmnt:
            {
                std::cout << "generating return!\n";
                retCodegen(ast);
                return;
            }
        case ANT::CompileUnit:
        case ANT::Program:
            {
                std::vector<AstNode*>* vec = ast->getChildren();
                for(auto c : (*vec)) {
                    generateIR_llvm(c);
                }
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

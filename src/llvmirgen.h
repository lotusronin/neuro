#ifndef LLVMIRGEN_H
#define LLVMIRGEN_H

#include "astnode.h"
#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/BasicBlock.h>

llvm::Type* getIRType(SemanticType t, std::string ident);
llvm::Value* expressionCodegen(AstNode* n);
llvm::Function* prototypeCodegen(AstNode* n);
llvm::Function* functionCodgen(AstNode* n);
llvm::Value* funcCallCodegen(AstNode* n);
llvm::Value* retCodegen(AstNode* n);
void blockCodegen(AstNode* n);
void vardecCodegen(AstNode* n);
void vardecassignCodegen(AstNode* n);
void assignCodegen(AstNode* n);
void ifelseCodegen(AstNode* n);
void whileloopCodegen(AstNode* n);
void statementCodegen(AstNode* n);
void generateIR_llvm(AstNode* ast);



#endif

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
#include "symboltable.h"

llvm::Type* getIRType(TypeInfo& info);
llvm::Type* getIRType(SemanticType t, std::string ident, int indirection);
llvm::StructType* getStructIRType(std::string ident);
llvm::Value* expressionCodegen(AstNode* n, SymbolTable* sym, bool lvalue=false);
llvm::Function* prototypeCodegen(AstNode* n, SymbolTable* sym);
llvm::Function* functionCodegen(AstNode* n, SymbolTable* sym);
llvm::Value* funcCallCodegen(AstNode* n, SymbolTable* sym);
llvm::Value* retCodegen(AstNode* n, SymbolTable* sym);
llvm::Value* conditionalCodegen(AstNode* n, SymbolTable* sym);
void blockCodegen(AstNode* n, llvm::BasicBlock* continueto, llvm::BasicBlock* breakto, SymbolTable* sym);
void vardecCodegen(AstNode* n, SymbolTable* sym);
void vardecassignCodegen(AstNode* n, SymbolTable* sym);
void assignCodegen(AstNode* n, SymbolTable* sym);
void ifelseCodegen(AstNode* n, llvm::BasicBlock* continueto, llvm::BasicBlock* breakto, SymbolTable* sym);
void whileloopCodegen(AstNode* n, SymbolTable* sym);
void forloopCodegen(AstNode* n, SymbolTable* sym);
void loopstmtCodegen(AstNode* n, llvm::BasicBlock* continueto, llvm::BasicBlock* breakto);
void statementCodegen(AstNode* n, llvm::BasicBlock* begin, llvm::BasicBlock* end, SymbolTable* sym);
void generateIR_llvm(AstNode* ast, SymbolTable* sym);



#endif

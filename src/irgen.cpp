#include <llvm/IR/Value.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include "astnode.h"

llvm::LLVMContext context;
llvm::IRBuilder<> Builder(context);
llvm::Module* module;

void generateIR(AstNode* ast) {
}

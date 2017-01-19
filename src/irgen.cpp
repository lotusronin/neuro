#include "irgen.h"
#include "llvmirgen.h"
#include <llvm/IR/Value.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Bitcode/ReaderWriter.h>
#include <iostream>
#include <string>
#include <assert.h>
#include "astnode.h"
#include "astnodetypes.h"
#include "symboltable.h"

using namespace llvm;

LLVMContext context;
IRBuilder<> Builder(context);
Module* module;

extern SymbolTable progSymTab;
extern std::unordered_map<std::string,std::unordered_map<std::string,int>*> userTypesList;

//TODO(marcus): make this a tree of mapped values.
std::map<std::string, AllocaInst*> varTable;

//TODO(marcus): need a map of struct name to struct members
//We need to know the index of a struct member for member access
//TODO(marcus): need a map of struct name to ir type
//In llvm 3.0, identified structs are not uniqued, so we have to keep track ourselves
//Currently type definitions are global to a project, but we may want to swap to
//local definitions? This would let us define two different types with the same name
//in two different files, maybe useful? But then we'd have to handle collisions if both
//those types are imported into a file...
std::map<std::string,StructType*> irTypeMap;


Type* getIRPtrType(Type* t, int indirection) {
    Type* ret = t;
    while(indirection > 0) {
        ret = PointerType::getUnqual(ret);
        indirection--;
    }
    return ret;
}

#define ST SemanticType
Type* getIRType(TypeInfo& info) {
    return getIRType(info.type,info.userid,info.indirection);
}

Type* getIRType(ST t, std::string ident = "", int indirection = 0) {
    Type* ret;
    switch(t) {
        case ST::Void:
            ret = Type::getVoidTy(context);
            break;
        case ST::Int:
        case ST::intlit:
            //TODO(marcus): intlit might not be i32
            ret = Type::getInt32Ty(context);
            break;
        case ST::Float:
        case ST::floatlit:
            //TODO(marcus): floatlit might not be float (double?)
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
        case ST::User:
            ret = getStructIRType(ident);
            //ret = StructType::create(context, ident);
            break;
        default:
            std::cout << "Type not supported, defaulting to void\n";
            if(t == ST::Infer) { std::cout << "Type should have been inferred!!\n";}
            ret = Type::getVoidTy(context);
            break;
    }
    
    if(indirection > 0) {
        //Handle pointers
        ret = getIRPtrType(ret, indirection);
    }

    return ret;
}

//TODO(marcus): should probably share this prototype in a header...
bool isPrimitiveType(ST type);
#undef ST

Type* generateTypeCodegen(AstNode* n) {
    std::cout << "Generating User Defined Type!\n";
    StructDefNode* sdnode = (StructDefNode*)n;
    std::cout << "Type: " << sdnode->ident << "\n";
    //Check to see if we already have an opaque struct defined
    StructType* ret = getStructIRType(sdnode->ident);
    if(ret == nullptr) {
        ret = StructType::create(context,sdnode->ident);
        irTypeMap.insert(std::pair<std::string,StructType*>(sdnode->ident,ret));
    }

    //If struct isn't opaque, report multiple definition, otherwise define body
    if(!ret->isOpaque()) {
        std::cout << "Error! multiple definitions of type " << sdnode->ident << " exists!\n";
        return ret;
    }

    //Get members of the type and construct it!
    std::vector<Type*> memberTypes;

    for(auto c : *(n->getChildren())) {
        auto var = (VarDecNode*)c;
        SemanticType stype = var->getRHS()->getType();
        int indirection = ((TypeNode*)var->getRHS())->mindirection;
        if(isPrimitiveType(stype)) {
            memberTypes.push_back(getIRType(stype,"",indirection));
        } else {
            auto typenode = (TypeNode*) var->getRHS();
            std::string usertypename = typenode->mtoken.token;
            auto userdeftype = getStructIRType(usertypename);
            userdeftype = (StructType*) getIRPtrType((Type*)userdeftype,indirection);
            if(userdeftype == nullptr) {
                userdeftype = StructType::create(context,usertypename);
                irTypeMap.insert(std::pair<std::string,StructType*>(usertypename,userdeftype));
            }
            memberTypes.push_back(userdeftype);
        }
    }

    ret->setBody(memberTypes);
    
    return ret;
}

StructType* getStructIRType(std::string ident) {
    StructType* ret = nullptr;
    auto iter = irTypeMap.find(ident);
    if(iter != irTypeMap.end()) {
        ret = iter->second;
    }
    return ret;
}


Function* prototypeCodegen(AstNode* n, SymbolTable* sym) {
    PrototypeNode* protonode = (PrototypeNode*) n;
    std::vector<AstNode*>* vec = protonode->getParameters();
    std::vector<Type*> parameterTypes;
    parameterTypes.reserve(vec->size());
   
    for(auto c : (*vec)) {
        ParamsNode* param_node = (ParamsNode*)c;
        auto sym_entry = getFirstEntry(sym, param_node->mname);
        assert(sym_entry != nullptr);
        TypeInfo info = sym_entry->typeinfo;
        Type* t = getIRType(info);
        parameterTypes.push_back(t);
    }

    auto sym_entries = getEntry(sym, protonode->mfuncname);
    auto entry = sym_entries.size() ? sym_entries[0] : nullptr;
    assert(entry != nullptr);
    Type* retType = getIRType(entry->typeinfo);
    
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

Function* functionCodgen(AstNode* n, SymbolTable* sym) {
    FuncDefNode* funcnode = (FuncDefNode*) n;
    std::cout << "Generating function " << funcnode->mfuncname << "\n";
    Function* F = module->getFunction(funcnode->mfuncname);
    std::vector<AstNode*>* vec = funcnode->getParameters();
    if(!F) {
        std::vector<Type*> parameterTypes;
        parameterTypes.reserve(vec->size());
       

        for(auto c : (*vec)) {
            ParamsNode* param_node = (ParamsNode*)c;
            auto sym_entry = getFirstEntry(sym, param_node->mname);
            assert(sym_entry != nullptr);
            TypeInfo info = sym_entry->typeinfo;
            Type* t = getIRType(info);
            parameterTypes.push_back(t);
        }
        
        auto sym_entries = getEntry(sym, funcnode->mfuncname);
        auto entry = sym_entries.size() ? sym_entries[0] : nullptr;
        assert(entry != nullptr);
        Type* retType = getIRType(entry->typeinfo);
        
        FunctionType* FT = FunctionType::get(retType, parameterTypes, false);
        F = Function::Create(FT, Function::ExternalLinkage, funcnode->mfuncname, module);

        unsigned int idx = 0;
        for(auto &Arg : F->args()) {
            std::string name = ((ParamsNode*) (*vec)[idx])->mname;
            Arg.setName(name);
            ++idx;
        }
    }

    //TODO(marcus): what is entry, can it be used for every function?
    //entry is a label for the basic block, llvm makes sure actual label is unique
    //by appending an number to the end
    BasicBlock* BB = BasicBlock::Create(context, "entry", F);
    Builder.SetInsertPoint(BB);

    for(auto &Arg : F->args()) {
        //std::cout << "Arg name: " << Arg.getName() << "\n";
        AllocaInst *alloca = Builder.CreateAlloca(Arg.getType(),0,Arg.getName());
        Builder.CreateStore(&Arg, alloca);
        //TODO(marcus): make sure to add these to the ir symbol table
        varTable[Arg.getName().str()] = alloca;
    }
    delete vec;
    return F;
}

Value* funcCallCodegen(AstNode* n, SymbolTable* sym) {
    FuncCallNode* callnode = (FuncCallNode*) n;
    Function* F = module->getFunction(callnode->mfuncname);
    if(!F) {
        std::cout << "Function lookup for " << callnode->mfuncname << " not found!\n";
        return nullptr;
    }

    std::vector<Value*> args;
    std::vector<AstNode*>* vec = callnode->getChildren();
    for(auto c : (*vec)) {
        //TODO(marcus): support multiple types
        auto exprval = expressionCodegen(c, sym);
        args.push_back(exprval);
    }

    Value* val;
    if(F->getReturnType() == Type::getVoidTy(context)) {
        val = Builder.CreateCall(F, args);
    } else {
        val = Builder.CreateCall(F, args, "calltemp");
    }
    return val;
}

Value* retCodegen(AstNode* n, SymbolTable* sym) {
    std::cout << "Generating return statement\n";
    ReturnNode* retnode = (ReturnNode*) n;
    auto pchildren = retnode->getChildren();
    Value* ret = nullptr;
    if(pchildren->size() > 0) {
        Value* retval = expressionCodegen((*retnode->getChildren())[0], sym);
        ret = Builder.CreateRet(retval);
    } else {
        ret = Builder.CreateRetVoid();
    }
    return ret;
}

#define ANT AstNodeType
Value* expressionCodegen(AstNode* n, SymbolTable* sym) {
    //TODO(marcus): Returning 0 isn't a great idea in the long run
    //if we fail to generate an expression we should report an error
    Value* val = ConstantInt::get(context, APInt(32,0));
    assert(n != nullptr);
    /*if(n == nullptr) {
        std::cout << "passed in nullptr\n";
        return val;
    }*/

    //TODO(marcus): support unary ops
    //TODO(marcus): support signed div
    //TODO(marcus): support floats
    //TODO(marcus): support type specific overloads
    //TODO(marcus): support not unsigned int constants
    //TODO(marcus): support different sizes
    //TODO(marcus): support different types
    switch(n->nodeType()) {
        case ANT::BinOp:
            {
            auto binop = (BinOpNode*)n;
            auto op = binop->getOp();
            if(op.compare("( )") == 0) {
                auto child = binop->LHS();
                return expressionCodegen(child, sym);
            }
            std::cout << "generating binop\n";
            auto lhs = binop->LHS();
            if(lhs->nodeType() == AstNodeType::Var) {
                std::cout << "child node is of type var!\n";
            }
            auto rhs = binop->RHS();
            Value* lhsv;
            Value* rhsv;
            if(op.compare(".")) {
                //don't generate if the op is a member access
                lhsv = expressionCodegen(lhs, sym);
                if(rhs) {
                    rhsv = expressionCodegen(rhs, sym);
                }
            }
            if(op.compare("+") == 0) {
                std::cout << "generating add\n";
                return Builder.CreateAdd(lhsv,rhsv,"addtemp");
            } else if(op.compare("-") == 0) {
                return Builder.CreateSub(lhsv,rhsv,"subtemp");
            } else if(op.compare("*") == 0) {
                return Builder.CreateMul(lhsv,rhsv,"multtemp");
            } else if(op.compare("/") == 0) {
                return Builder.CreateUDiv(lhsv,rhsv,"divtemp");
            } else if(op.compare("%") == 0) {
                //TODO(marcus): support signed and floating remainder
                return Builder.CreateURem(lhsv,rhsv,"modtemp");
            } else if(op.compare(">") == 0) {
                return Builder.CreateICmpUGT(lhsv,rhsv,"gttemp");
            } else if(op.compare("<") == 0) {
                return Builder.CreateICmpULT(lhsv,rhsv,"lttemp");
            } else if(op.compare(">=") == 0) {
                return Builder.CreateICmpUGE(lhsv,rhsv,"getemp");
            } else if(op.compare("<=") == 0) {
                return Builder.CreateICmpULE(lhsv,rhsv,"letemp");
            } else if(op.compare("==") == 0) {
                return Builder.CreateICmpEQ(lhsv,rhsv,"eqtemp");
            } else if(op.compare("!=") == 0) {
                return Builder.CreateICmpNE(lhsv,rhsv,"neqtemp");
            } else if(op.compare("|") == 0) {
                return Builder.CreateOr(lhsv,rhsv,"bitortemp");
            } else if(op.compare("^") == 0) {
                return Builder.CreateXor(lhsv,rhsv,"bitxortemp");
            } else if(op.compare("~") == 0) {
                //NOTE(marcus): bitwise ops in llvm only work for integers or vectors of integers
                //TODO(marcus): make this work for other sizes of integers
                return Builder.CreateXor(lhsv,ConstantInt::get(context,APInt(32,-1)));
            } else if(op.compare("||") == 0) {
                //TODO(marcus): make this work for other sizes of integers
                auto res = Builder.CreateAlloca(Type::getInt1Ty(context),0,"or_res.addr"); 
                auto zero_val = ConstantInt::get(context, APInt(32,0));
                auto firstval = Builder.CreateICmpNE(lhsv,zero_val);
                Builder.CreateStore(firstval,res);
                auto enclosingscope = Builder.GetInsertBlock()->getParent();
                BasicBlock* secBB = BasicBlock::Create(context, "orsecond");
                enclosingscope->getBasicBlockList().push_back(secBB);
                BasicBlock* endBB = BasicBlock::Create(context, "orend");
                enclosingscope->getBasicBlockList().push_back(endBB);
                Builder.CreateCondBr(firstval,endBB,secBB);
                Builder.SetInsertPoint(secBB);
                auto secondval = Builder.CreateICmpNE(rhsv,zero_val);
                Builder.CreateStore(secondval,res);
                Builder.CreateBr(endBB);
                Builder.SetInsertPoint(endBB);
                return Builder.CreateLoad(res,"or_res");
            } else if(op.compare("&&") == 0) {
                //TODO(marcus): make this work for other sizes of integers
                auto res = Builder.CreateAlloca(Type::getInt1Ty(context),0,"and_res.addr"); 
                auto zero_val = ConstantInt::get(context, APInt(32,0));
                auto firstval = Builder.CreateICmpNE(lhsv,zero_val);
                Builder.CreateStore(firstval,res);
                auto enclosingscope = Builder.GetInsertBlock()->getParent();
                BasicBlock* secBB = BasicBlock::Create(context, "andsecond");
                enclosingscope->getBasicBlockList().push_back(secBB);
                BasicBlock* endBB = BasicBlock::Create(context, "andend");
                enclosingscope->getBasicBlockList().push_back(endBB);
                Builder.CreateCondBr(firstval,secBB,endBB);
                Builder.SetInsertPoint(secBB);
                auto secondval = Builder.CreateICmpNE(rhsv,zero_val);
                Builder.CreateStore(secondval,res);
                Builder.CreateBr(endBB);
                Builder.SetInsertPoint(endBB);
                return Builder.CreateLoad(res,"and_res");
            } else if(op.compare(".") == 0) {
                std::cout << "Generating member access!\n";
                std::cout << "Var name " << lhs->mtoken.token << "\n";
                std::cout << "Var name is also..." << ((VarNode*)lhs)->getVarName() << "\n";
                auto structname = ((VarNode*)lhs)->getVarName();
                auto lhsv = varTable[structname];
                //FIXME(marcus): need a cleaner way to get address of variables
                //should use the symbol table instead.
                //Currently it generates a load of the lefthand side variable
                //so we can get the type!
                //NOTE(marcus): only way for something to have an address is if we alloca it?
                //Then we may need to alloca temp variables if part of an expression
                //generates a struct that we will do address of or member access on.
                auto structtype = expressionCodegen(lhs,sym)->getType();
                //auto structtype = lhsv->getType();
                if(structtype->isPointerTy()) {
                    std::cout << "struct is actually a pointer type!\n";
                } else {
                    std::cout << "struct is not a pointer type\n";
                }
                //auto structmembername = rhs->mtoken.token;
                auto structmembername = ((VarNode*)rhs)->getVarName();
                std::cout << "member name is " << structmembername << "\n";
                auto type_name = structtype->getStructName();
                auto memberlist = userTypesList.find(type_name.str());
                if(memberlist != userTypesList.end()) {
                    auto memberind = memberlist->second->find(structmembername);
                    if(memberind != memberlist->second->end()) {
                        unsigned int index = memberind->second;
                        //auto indexval = ConstantInt::get(context,APInt(32,index));
                        std::cout << "Success, getting element index " << index << "\n";
                        //return Builder.CreateGEP(structtype,lhsv,indexval,structmembername);
                        auto memberptr = Builder.CreateStructGEP(structtype,lhsv,index,"ptr_"+structmembername);
                        return Builder.CreateLoad(memberptr,structmembername);

                    } else {
                        std::cout << "Error, member " << structmembername << " isn't a known member of struct " << type_name.str() << "\n";
                    }
                } else {
                    std::cout << "Error, member access on variable with unregistered struct type\n";
                }
                return val;

            } else if(op.compare("@") == 0) {
                //TODO(marcus): we are assuming the expression underneath will be just a variable of
                //some pointer type
                auto childvar = expressionCodegen(lhs,sym);
                return Builder.CreateLoad(childvar,"deref");
            } else if(op.compare("&") == 0) {
                std::cout << "Generating address of\n";
                //TODO(marcus): we are assuming the expression underneath will be some variable
                //which has been allocated on the stack.
                if(lhs->nodeType() == AstNodeType::Var) {
                    std::cout << "We have a var to take the address of!\n";
                    auto varname = ((VarNode*)lhs)->getVarName();
                    return varTable[varname];
                }
                //TODO(marcus): this is wrong but will need to fix storing of variables for IR
                //generation first. Maybe its own symbol table?
                auto childvar = expressionCodegen(lhs,sym);
                return childvar;
            } else {
                std::cout << "Unknown binary expression" << op << "\n";
                return val;
            }
            }
            break;
        case ANT::Const:
            {
                std::cout << "generating constant!\n";
                auto constn = (ConstantNode*)n;
                auto strval = constn->getVal();
                if(constn->mstype == SemanticType::Char && constn->mtypeinfo.indirection == 1) {
                    val = Builder.CreateGlobalStringPtr(strval, "g_str");
                } else {
                    int constval = std::stoi(strval);
                    val = ConstantInt::get(context, APInt(32,constval));
                }
            }
            break;
        case ANT::Var:
            {
                std::cout << "generating varload!\n";
                auto varn = (VarNode*)n;
                auto varloc = varTable[varn->getVarName()];
                if(varloc == nullptr) std::cout << "NULLPTR!!!\n";
                auto varv = Builder.CreateLoad(varloc,varn->getVarName());
                val = varv;
            }
            break;
        case ANT::FuncCall:
            {
                val = funcCallCodegen(n,sym);
            }
            break;
        default:
            std::cout << "default case for expression generation\n";
            break;
    }
    return val;
}
#undef ANT

void blockCodegen(AstNode* n, BasicBlock* continueto, BasicBlock* breakto, SymbolTable* sym) {
    std::cout << "generating block\n";
    auto scope = getScope(sym, "block"+std::to_string(((BlockNode*)n)->getId()));
    std::vector<AstNode*>* vec = n->getChildren();
    for(auto c : (*vec)) {
        statementCodegen(c, continueto, breakto, scope);
    }
}

//TODO(marcus): see if this code needs to be merged into one function later
void vardecCodegen(AstNode* n, SymbolTable* sym) {
    auto vardecn = (VarDecNode*) n;
    auto varn = (VarNode*)vardecn->mchildren.at(0);
    auto symbol_table_entry = getFirstEntry(sym,varn->getVarName());
    auto typeinfo = symbol_table_entry->typeinfo;
    //TODO(marcus): fix how you access the name of the variable
    auto irtype = getIRType(typeinfo);
    auto alloc = Builder.CreateAlloca(irtype,0,varn->getVarName());
    varTable[varn->getVarName()] = alloc;
    return;
}

void vardecassignCodegen(AstNode* n, SymbolTable* sym) {
    std::cout << "Generating vardec assign\n";
    auto vardecan = (VarDecAssignNode*) n;
    auto varn = (VarNode*)vardecan->mchildren.at(0);
    auto symbol_table_entry = getFirstEntry(sym,varn->getVarName());
    auto typeinfo = symbol_table_entry->typeinfo;
    std::cout << typeinfo << '\n';
    auto ir_type = getIRType(typeinfo);
    //TODO(marcus): fix how you access the name of the variable
    AllocaInst* alloca = Builder.CreateAlloca(ir_type,0,varn->getVarName());
    varTable[varn->getVarName()] = alloca;
    //TODO(marcus): don't hardcode child accesses
    Value* val = expressionCodegen(vardecan->mchildren.at(1), sym);
    std::cout << "generating store for assignment\n";
    Builder.CreateStore(val,alloca);
    return;
}

void assignCodegen(AstNode* n, SymbolTable* sym) {
    std::cout << "Generating assingment\n";
    //TODO(marcus): don't hardcode child access
    auto assignn = (AssignNode*)n;
    auto varn = (VarNode*) assignn->mchildren.at(0);
    auto rhs = assignn->mchildren.at(1);
    //TODO(marcus): make left hand side work for expressions that dereference an address
    //(arrays, *ptrs, member access)
    Value* var = varTable[varn->getVarName()];
    Value* val = expressionCodegen(rhs,sym);
    Builder.CreateStore(val,var);
    return;
}

void ifelseCodegen(AstNode* n, BasicBlock* continueto, BasicBlock* breakto, SymbolTable* sym) {
    std::cout << "Generating if statement\n";
    auto ifn = (IfNode*) n;
    auto enclosingscope = Builder.GetInsertBlock()->getParent();
    BasicBlock* thenBB = BasicBlock::Create(context, "then", enclosingscope);
    BasicBlock* elseBB = BasicBlock::Create(context, "else");
    BasicBlock* mergeBB = BasicBlock::Create(context, "merge");
    auto condv = conditionalCodegen(ifn->getConditional(), sym);
    
    Builder.CreateCondBr(condv,thenBB,elseBB);

    Builder.SetInsertPoint(thenBB);
    statementCodegen(ifn->getThen(), continueto, breakto, sym);

    Builder.CreateBr(mergeBB);
    enclosingscope->getBasicBlockList().push_back(elseBB);
    Builder.SetInsertPoint(elseBB);
    if(ifn->mstatements.size() == 3) {
        //TODO(marcus): don't hardcode child access
        auto elsen = (ElseNode*) ifn->mstatements.at(2);
        statementCodegen(elsen->mstatements.at(0), continueto, breakto, sym);
    }
    Builder.CreateBr(mergeBB);
    enclosingscope->getBasicBlockList().push_back(mergeBB);
    Builder.SetInsertPoint(mergeBB);
    return;
}

void whileloopCodegen(AstNode* n, SymbolTable* sym) {
    auto whilen = (WhileLoopNode*) n;
    auto enclosingscope = Builder.GetInsertBlock()->getParent();
    BasicBlock* whileBB = BasicBlock::Create(context,"while",enclosingscope);
    BasicBlock* beginBB = BasicBlock::Create(context,"whilebegin",enclosingscope);
    BasicBlock* endBB = BasicBlock::Create(context,"whileend",enclosingscope);

    Builder.CreateBr(whileBB);
    Builder.SetInsertPoint(whileBB);

    auto condv = conditionalCodegen(whilen->getConditional(), sym);
    Builder.CreateCondBr(condv,beginBB,endBB);
    Builder.SetInsertPoint(beginBB);

    statementCodegen(whilen->getBody(), whileBB, endBB, sym);
    
    Builder.CreateBr(whileBB);
    Builder.SetInsertPoint(endBB);
    return;
}

void forloopCodegen(AstNode* n, SymbolTable* sym) {
    auto forn = (ForLoopNode*) n;
    auto inits = forn->getInit();
    auto condition = forn->getConditional();
    auto update = forn->getUpdate();
    auto loopbody = forn->getBody();
    auto enclosingscope = Builder.GetInsertBlock()->getParent();
    BasicBlock* forBB = BasicBlock::Create(context,"for",enclosingscope);
    BasicBlock* beginBB = BasicBlock::Create(context,"forbegin",enclosingscope);
    BasicBlock* bodyBB = BasicBlock::Create(context,"forbody",enclosingscope);
    BasicBlock* updateBB = BasicBlock::Create(context,"forupdate",enclosingscope);
    BasicBlock* endBB = BasicBlock::Create(context,"forend",enclosingscope);
    
    Builder.CreateBr(forBB);
    Builder.SetInsertPoint(forBB);
    statementCodegen(inits, updateBB, endBB, sym);
    Builder.CreateBr(beginBB);
    Builder.SetInsertPoint(beginBB);
    auto condv = conditionalCodegen(condition, sym);
    Builder.CreateCondBr(condv,bodyBB,endBB);
    Builder.SetInsertPoint(bodyBB);
    statementCodegen(loopbody, updateBB, endBB, sym);
    Builder.CreateBr(updateBB);
    Builder.SetInsertPoint(updateBB);
    statementCodegen(update, updateBB, endBB, sym);
    Builder.CreateBr(beginBB);
    Builder.SetInsertPoint(endBB);

}

Value* conditionalCodegen(AstNode* n, SymbolTable* sym) {
    //TODO(marcus): support other types
    auto temp_condv = expressionCodegen(n, sym);
    auto condv = temp_condv;
    if(!temp_condv->getType()->isIntegerTy(1)) {
        Value* zero = ConstantInt::get(temp_condv->getType(),0);
        condv = Builder.CreateICmpNE(temp_condv,zero,"condv");
    }
    return condv;
}

void loopstmtCodegen(AstNode* n, BasicBlock* continueto, BasicBlock* breakto) {
    BasicBlock* loc;
    if(n->mtoken.token.compare("break") == 0) {
        std::cout << "Generating break! LOOP STATEMENT!\n";
        loc = breakto;
    } else {
        std::cout << "Generating continue! LOOP STATEMENT\n";
        loc = continueto;
    }
    std::cout << n->mtoken.token << "\n";

    if(loc != nullptr) {
        Builder.CreateBr(loc);
    }
    return;
}

#define ANT AstNodeType
void statementCodegen(AstNode* n, BasicBlock* begin=nullptr, BasicBlock* end=nullptr, SymbolTable* sym=nullptr) {
    switch(n->nodeType()) {
        case ANT::RetStmnt:
                retCodegen(n, sym);
                break;
        case ANT::Block:
                blockCodegen(n, begin, end, sym);
                break;
        case ANT::VarDec:
                vardecCodegen(n, sym);
                break;
        case ANT::VarDecAssign:
                vardecassignCodegen(n, sym);
                break;
        case ANT::Assign:
                assignCodegen(n, sym);
                break;
        case ANT::IfStmt:
                ifelseCodegen(n, begin, end, sym);
                break;
        case ANT::WhileLoop:
                whileloopCodegen(n, sym);
                break;
        case ANT::FuncCall:
                funcCallCodegen(n, sym);
                break;
        case ANT::ForLoop:
                forloopCodegen(n, sym);
                break;
        case ANT::LoopStmt:
                loopstmtCodegen(n,begin,end);
                break;
        default:
            //std::cout << "Unknown node type\n";
            std::cout << "defaulting to expression\n";
                expressionCodegen(n,sym);
            break;
    }
}

void generateIR_llvm(AstNode* ast, SymbolTable* sym) {
    
    //check for null
    if(!ast)
        return;

    //Handle IR gen for each node type
    switch(ast->nodeType()) {
        case ANT::Prototype:
            {
                auto scope = getScope(sym, ((FuncDefNode*)ast)->mfuncname);
                Function* f = prototypeCodegen(ast, scope);
                //f->dump();
                return;
            }
            break;
        case ANT::FuncDef:
            {
                auto scope = getScope(sym, ((FuncDefNode*)ast)->mfuncname);
                Function* F = functionCodgen(ast, scope);
               statementCodegen(((FuncDefNode*)ast)->getFunctionBody(),nullptr,nullptr,scope);
               return;
            }
            break;
        case ANT::FuncCall:
            {
                //TODO(marcus): This is an unused cases.
                std::cout << "generating function call\n";
                funcCallCodegen(ast, sym);
                return;
            }
            break;
        case ANT::RetStmnt:
            {
                //TODO(marcus): This is an unused cases.
                std::cout << "generating return!\n";
                retCodegen(ast, sym);
                return;
            }
            break;
        case ANT::CompileUnit:
            {
                auto scope = getScope(sym, ((CompileUnitNode*)ast)->getFileName());
                std::vector<AstNode*>* vec = ast->getChildren();
                for(auto c : (*vec)) {
                    generateIR_llvm(c,scope);
                }
                return;
            }
            break;
        case ANT::Program:
            {
                std::vector<AstNode*>* vec = ast->getChildren();
                for(auto c : (*vec)) {
                    generateIR_llvm(c,sym);
                }
                return;
            }
            break;
        case ANT::StructDef:
            {
                generateTypeCodegen(ast);
                return;
            }
            break;
        default:
            break;
    }

    //recurse
    std::vector<AstNode*>* vec = ast->getChildren();
    for(auto c : (*vec)) {
        generateIR_llvm(c, sym);
    }
}

void dumpIR() {
    module->dump();
    return;
}

void writeIR(std::string o) {
    std::string out = o + ".ll";
    std::error_code EC;
    raw_fd_ostream dest(out,EC,sys::fs::F_Text);
    module->print(dest,nullptr,false,true);
    dest.flush();
}

void generateIR(AstNode* ast) {
    module = new Module("Neuro Program", context);
    generateIR_llvm(ast, &progSymTab);
}

void writeObj(std::string o) {
    std::string out = o + ".o";
  
    InitializeNativeTarget();
    InitializeNativeTargetAsmParser();
    InitializeNativeTargetAsmPrinter();

    //TODO(marcus): allow custom targets/options!
    auto TargetTriple = sys::getDefaultTargetTriple();
    std::cout << "Default target triple is... " << TargetTriple << '\n';
    module->setTargetTriple(TargetTriple);
    std::string error;
    auto Target = TargetRegistry::lookupTarget(TargetTriple,error);
    if(!Target) {
        std::cout << error << "\n";
        return;
    }

    auto CPU = "generic";
    auto Features = "";

    TargetOptions opt;
    auto RM = Reloc::Model();
    auto TheTargetMachine = Target->createTargetMachine(TargetTriple,CPU,Features,opt,RM);

    module->setDataLayout(TheTargetMachine->createDataLayout());

    std::error_code EC;
    raw_fd_ostream dest(out,EC,sys::fs::F_None);
    if(EC) {
        errs() << "Could not open file : " << EC.message();
        return;
    }

    legacy::PassManager pass;
    auto FileType = TargetMachine::CGFT_ObjectFile;

    if(TheTargetMachine->addPassesToEmitFile(pass, dest, FileType)) {
        errs() << "TheTargetMachine can't emit file!\n";
        return;
    }

    pass.run(*module);
    dest.flush();

    outs() << "Wrote " << out << "\n";
    return;
}

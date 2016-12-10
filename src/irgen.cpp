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
#include "astnode.h"
#include "astnodetypes.h"
#include "symboltable.h"

using namespace llvm;

LLVMContext context;
IRBuilder<> Builder(context);
Module* module;

extern SymbolTable progSymTab;

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
        case ST::User:
            ret = getStructIRType(ident);
            //ret = StructType::create(context, ident);
            break;
        default:
            std::cout << "Type not supported, defaulting to void\n";
            ret = Type::getVoidTy(context);
            break;
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
        if(isPrimitiveType(stype)) {
            memberTypes.push_back(getIRType(stype));
        } else {
            auto typenode = (TypeNode*) var->getRHS();
            std::string usertypename = typenode->mtoken.token;
            auto userdeftype = getStructIRType(usertypename);
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


Function* prototypeCodegen(AstNode* n) {
    PrototypeNode* protonode = (PrototypeNode*) n;
    std::vector<AstNode*>* vec = protonode->getParameters();
    std::vector<Type*> parameterTypes;
    parameterTypes.reserve(vec->size());
   
    for(auto c : (*vec)) {
        //TODO(marcus): Make sure params pull type from their child
        auto typenode = c->getChildren()->at(0);
        auto node_semantic_type = typenode->getType();
        std::string type_string = typenode->mtoken.token;
        Type* t = getIRType(node_semantic_type, type_string);
        //Type* t = getIRType(SemanticType::Int);
        parameterTypes.push_back(t);
    }
    
    //TODO(marcus): find a better way to get type name...
    Type* retType = getIRType(protonode->getType(), protonode->getChildren()->back()->mtoken.token);
    
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
    std::cout << "Generating function " << funcnode->mfuncname << "\n";
    Function* F = module->getFunction(funcnode->mfuncname);
    std::vector<AstNode*>* vec = funcnode->getParameters();
    if(!F) {
        std::vector<Type*> parameterTypes;
        parameterTypes.reserve(vec->size());
       

        for(auto c : (*vec)) {
            //TODO(marcus): don't hardcode child accesses
            ParamsNode* param_node = (ParamsNode*)c;
            TypeNode* param_type = (TypeNode*) param_node->mchildren.at(0);
            //TODO(marcus): get a better way to get type name
            Type* t = getIRType(param_type->getType(), param_type->mtoken.token);
            parameterTypes.push_back(t);
        }
        
        //TODO(marcus): find a better way to get type name...
        Type* retType = getIRType(funcnode->getType(), funcnode->getChildren()->back()->mtoken.token);
        
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
        varTable[Arg.getName()] = alloca;
    }
    delete vec;
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
        //TODO(marcus): support multiple types
        auto exprval = expressionCodegen(c);
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

Value* retCodegen(AstNode* n) {
    std::cout << "Generating return statement\n";
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

#define ANT AstNodeType
Value* expressionCodegen(AstNode* n) {
    //TODO(marcus): Returning 0 isn't a great idea in the long run
    //if we fail to generate an expression we should report an error
    Value* val = ConstantInt::get(context, APInt(32,0));
    if(n == nullptr) {
        std::cout << "passed in nullptr\n";
        return val;
    }

    //TODO(marcus): support unary ops
    //TODO(marcus): support signed div
    //TODO(marcus): support floats
    //TODO(marcus): support type specific overloads
    //TODO(marcus): support function calls
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
                return expressionCodegen(child);
            }
            std::cout << "generating binop\n";
            auto lhs = binop->LHS();
            auto rhs = binop->RHS();
            auto lhsv = expressionCodegen(lhs);
            auto rhsv = expressionCodegen(rhs);
            if(op.compare("+") == 0) {
                std::cout << "generating add\n";
                return Builder.CreateAdd(lhsv,rhsv,"addtemp");
            } else if(op.compare("-") == 0) {
                return Builder.CreateSub(lhsv,rhsv,"subtemp");
            } else if(op.compare("*") == 0) {
                return Builder.CreateMul(lhsv,rhsv,"multtemp");
            } else if(op.compare("/") == 0) {
                return Builder.CreateUDiv(lhsv,rhsv,"divtemp");
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
                int constval = std::stoi(strval);
                val = ConstantInt::get(context, APInt(32,constval));
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
                val = funcCallCodegen(n);
            }
            break;
        default:
            std::cout << "default case for expression generation\n";
            break;
    }
    return val;
}
#undef ANT

void blockCodegen(AstNode* n) {
    std::cout << "generating block\n";
    std::vector<AstNode*>* vec = n->getChildren();
    for(auto c : (*vec)) {
        statementCodegen(c);
    }
}

//TODO(marcus): see if this code needs to be merged into one function later
void vardecCodegen(AstNode* n) {
    auto vardecn = (VarDecNode*) n;
    auto varn = (VarNode*)vardecn->mchildren.at(0);
    auto type_node = (TypeNode*)vardecn->getRHS();
    //TODO(marcus): fix how you access the name of the variable
    auto alloc = Builder.CreateAlloca(getIRType(type_node->getType(),type_node->mtoken.token),0,varn->getVarName());
    varTable[varn->getVarName()] = alloc;
    return;
}

void vardecassignCodegen(AstNode* n) {
    std::cout << "Generating vardec assign\n";
    auto vardecan = (VarDecAssignNode*) n;
    auto varn = (VarNode*)vardecan->mchildren.at(0);
    //FIXME(marcus): get the type of the node once type checking works
    //TODO(marcus): fix how you access the name of the variable
    AllocaInst* alloca = Builder.CreateAlloca(Type::getInt32Ty(context),0,varn->getVarName());
    varTable[varn->getVarName()] = alloca;
    //TODO(marcus): don't hardcode child accesses
    Value* val = expressionCodegen(vardecan->mchildren.at(1));
    std::cout << "generating store for assignment\n";
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

void ifelseCodegen(AstNode* n) {
    std::cout << "Generating if statement\n";
    auto ifn = (IfNode*) n;
    auto enclosingscope = Builder.GetInsertBlock()->getParent();
    BasicBlock* thenBB = BasicBlock::Create(context, "then", enclosingscope);
    BasicBlock* elseBB = BasicBlock::Create(context, "else");
    BasicBlock* mergeBB = BasicBlock::Create(context, "merge");
    auto condv = conditionalCodegen(ifn->getConditional());
    
    Builder.CreateCondBr(condv,thenBB,elseBB);

    Builder.SetInsertPoint(thenBB);
    statementCodegen(ifn->getThen());

    Builder.CreateBr(mergeBB);
    enclosingscope->getBasicBlockList().push_back(elseBB);
    Builder.SetInsertPoint(elseBB);
    if(ifn->mstatements.size() == 3) {
        //TODO(marcus): don't hardcode child access
        auto elsen = (ElseNode*) ifn->mstatements.at(2);
        statementCodegen(elsen->mstatements.at(0));
    }
    Builder.CreateBr(mergeBB);
    enclosingscope->getBasicBlockList().push_back(mergeBB);
    Builder.SetInsertPoint(mergeBB);
    return;
}

void whileloopCodegen(AstNode* n) {
    auto whilen = (WhileLoopNode*) n;
    auto enclosingscope = Builder.GetInsertBlock()->getParent();
    BasicBlock* whileBB = BasicBlock::Create(context,"while",enclosingscope);
    BasicBlock* beginBB = BasicBlock::Create(context,"whilebegin",enclosingscope);
    BasicBlock* endBB = BasicBlock::Create(context,"whileend",enclosingscope);

    Builder.CreateBr(whileBB);
    Builder.SetInsertPoint(whileBB);

    auto condv = conditionalCodegen(whilen->getConditional());
    Builder.CreateCondBr(condv,beginBB,endBB);
    Builder.SetInsertPoint(beginBB);

    statementCodegen(whilen->getBody());
    
    Builder.CreateBr(whileBB);
    Builder.SetInsertPoint(endBB);
    return;
}

void forloopCodegen(AstNode* n) {
    auto forn = (ForLoopNode*) n;
    auto inits = forn->getInit();
    auto condition = forn->getConditional();
    auto update = forn->getUpdate();
    auto loopbody = forn->getBody();
    auto enclosingscope = Builder.GetInsertBlock()->getParent();
    BasicBlock* forBB = BasicBlock::Create(context,"for",enclosingscope);
    BasicBlock* beginBB = BasicBlock::Create(context,"forbegin",enclosingscope);
    BasicBlock* bodyBB = BasicBlock::Create(context,"forbody",enclosingscope);
    BasicBlock* endBB = BasicBlock::Create(context,"forend",enclosingscope);
    
    Builder.CreateBr(forBB);
    Builder.SetInsertPoint(forBB);
    statementCodegen(inits);
    Builder.CreateBr(beginBB);
    Builder.SetInsertPoint(beginBB);
    auto condv = conditionalCodegen(condition);
    Builder.CreateCondBr(condv,bodyBB,endBB);
    Builder.SetInsertPoint(bodyBB);
    statementCodegen(loopbody);
    statementCodegen(update);
    Builder.CreateBr(beginBB);
    Builder.SetInsertPoint(endBB);

}

Value* conditionalCodegen(AstNode* n) {
    //TODO(marcus): support other types
    auto temp_condv = expressionCodegen(n);
    auto condv = temp_condv;
    if(!temp_condv->getType()->isIntegerTy(1)) {
        Value* zero = ConstantInt::get(temp_condv->getType(),0);
        condv = Builder.CreateICmpNE(temp_condv,zero,"condv");
    }
    return condv;
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
        case ANT::IfStmt:
                ifelseCodegen(n);
                break;
        case ANT::WhileLoop:
                whileloopCodegen(n);
                break;
        case ANT::FuncCall:
                funcCallCodegen(n);
                break;
        case ANT::ForLoop:
                forloopCodegen(n);
                break;
        default:
            std::cout << "Unknown node type\n";
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
            break;
        case ANT::RetStmnt:
            {
                std::cout << "generating return!\n";
                retCodegen(ast);
                return;
            }
            break;
        case ANT::CompileUnit:
        case ANT::Program:
            {
                std::vector<AstNode*>* vec = ast->getChildren();
                for(auto c : (*vec)) {
                    generateIR_llvm(c);
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
        generateIR_llvm(c);
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
    generateIR_llvm(ast);
}

void writeObj(std::string o) {
    std::string out = o + ".o";
  
    InitializeNativeTarget();
    InitializeNativeTargetAsmParser();
    InitializeNativeTargetAsmPrinter();

    //TODO(marcus): allow custom targets/options!
    auto TargetTriple = sys::getDefaultTargetTriple();
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

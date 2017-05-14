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
//#include <llvm/Bitcode/ReaderWriter.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/IR/Verifier.h>
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
        case ST::u32:
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

//TODO(marcus): do a better check for primitive types (pointers?)
bool isPrimitiveType(TypeInfo t) {
    bool ret = true;
    ST type = t.type;
    switch(type) {
        case ST::User:
            ret = false;
        default:
            break;
    }
    return ret;
}
#undef ST

Type* generateTypeCodegen(AstNode* n) {
    //std::cout << "Generating User Defined Type!\n";
    StructDefNode* sdnode = (StructDefNode*)n;
    //std::cout << "Type: " << sdnode->ident << "\n";
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
    
    if(sdnode->foreign) {
        return ret;
    }

    //Get members of the type and construct it!
    std::vector<Type*> memberTypes;

    for(auto c : *(n->getChildren())) {
        auto var = (VarDecNode*)c;
        TypeInfo typeinfo = var->mtypeinfo; 
        SemanticType stype = typeinfo.type;
        int indirection = typeinfo.indirection;
        if(isPrimitiveType(typeinfo)) {
            memberTypes.push_back(getIRType(stype,"",indirection));
        } else {
            //TODO(marcus): make this work for non struct ptrs
            std::string usertypename = typeinfo.userid;
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
    Function* defined = module->getFunction(protonode->mfuncname);
    if(defined) {
        return defined;
    }
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

Function* functionCodegen(AstNode* n, SymbolTable* sym, bool prepass) {
    FuncDefNode* funcnode = (FuncDefNode*) n;
    //std::cout << "Generating function " << funcnode->mfuncname << "\n";
    std::string mangledName = funcnode->mangledName();
    Function* F = module->getFunction(mangledName);
    std::vector<AstNode*>* vec = funcnode->getParameters();
    if(!F) {
        funcnode->mangledName();
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
        
        auto sym_entries = getEntry(sym, funcnode->mfuncname+std::to_string(funcnode->id));
        auto entry = sym_entries.size() ? sym_entries[0] : nullptr;
        assert(entry != nullptr);
        Type* retType = getIRType(entry->typeinfo);
        
        FunctionType* FT = FunctionType::get(retType, parameterTypes, false);
        F = Function::Create(FT, Function::ExternalLinkage, mangledName, module);

        unsigned int idx = 0;
        for(auto &Arg : F->args()) {
            std::string name = ((ParamsNode*) (*vec)[idx])->mname;
            Arg.setName(name);
            ++idx;
        }
    }

    if(prepass) {
        delete vec;
        return F;
    }

    //NOTE(marcus): entry is a label for the basic block, llvm makes sure actual label is unique
    BasicBlock* BB = BasicBlock::Create(context, "entry", F);
    Builder.SetInsertPoint(BB);

    for(auto &Arg : F->args()) {
        //std::cout << "Arg name: " << Arg.getName() << "\n";
        AllocaInst *alloca = Builder.CreateAlloca(Arg.getType(),0,Arg.getName()+".addr");
        Builder.CreateStore(&Arg, alloca);
        auto sym_entry = getFirstEntry(sym, Arg.getName());
        sym_entry->address = (void*) alloca;
    }
    delete vec;
    return F;
}

Value* funcCallCodegen(AstNode* n, SymbolTable* sym) {
    FuncCallNode* callnode = (FuncCallNode*) n;
    Function* F = module->getFunction(callnode->mfuncname);
    if(!F) {
        //std::cout << "Function lookup for " << callnode->mfuncname << " not found!\n";
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
    //std::cout << "Generating return statement\n";
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
Value* expressionCodegen(AstNode* n, SymbolTable* sym, bool lvalue) {
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
            //std::cout << "generating binop\n";
            auto lhs = binop->LHS();
            if(lhs->nodeType() == AstNodeType::Var) {
                //std::cout << "child node is of type var!\n";
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
                if(lhs->mtypeinfo.indirection) {
                    return Builder.CreateGEP(lhsv,rhsv,"ptr");
                } else if(rhs->mtypeinfo.indirection) {
                    return Builder.CreateGEP(rhsv,lhsv,"ptr");
                }
                //std::cout << "generating add\n";
                return Builder.CreateAdd(lhsv,rhsv,"addtemp");
            } else if(op.compare("-") == 0) {
                if(binop->unaryOp) {
                    //TODO(marcus): make this work for other sizes
                    rhsv = ConstantInt::get(context, APInt(32,-1));
                    return Builder.CreateMul(lhsv,rhsv);
                }
                if(lhs->mtypeinfo.indirection) {
                    return Builder.CreateGEP(lhsv,rhsv,"ptr");
                } else if(rhs->mtypeinfo.indirection) {
                    return Builder.CreateGEP(rhsv,lhsv,"ptr");
                }
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
                if(lhs->mtypeinfo.type == SemanticType::Int) {
                    return Builder.CreateICmpSLT(lhsv,rhsv,"lttemp");
                }
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
                Value* zero_val;
                if(lhsv->getType()->isIntegerTy(1)) {
                    zero_val = ConstantInt::get(context, APInt(1,0));
                } else {
                    zero_val = ConstantInt::get(context, APInt(32,0));
                }
                auto firstval = Builder.CreateICmpNE(lhsv,zero_val);
                auto secondval = Builder.CreateICmpNE(rhsv,zero_val);
                auto oredvals = Builder.CreateOr(firstval,secondval,"or_res");
                return oredvals;
            } else if(op.compare("&&") == 0) {
                //TODO(marcus): make this work for other sizes of integers
                Value* zero_val;
                if(lhsv->getType()->isIntegerTy(1)) {
                    zero_val = ConstantInt::get(context, APInt(1,0));
                } else {
                    zero_val = ConstantInt::get(context, APInt(32,0));
                }
                auto firstval = Builder.CreateICmpNE(lhsv,zero_val);
                auto secondval = Builder.CreateICmpNE(rhsv,zero_val);
                auto andedvals = Builder.CreateAnd(firstval,secondval,"and_res");
                return andedvals;
            } else if(op.compare(".") == 0) {
                //std::cout << "Generating member access!\n";
                //std::cout << "Var name " << lhs->mtoken.token << "\n";
                //std::cout << "Var name is also..." << ((VarNode*)lhs)->getVarName() << "\n";
                auto structname = ((VarNode*)lhs)->getVarName();
                auto sym_entry = getFirstEntry(sym, structname);
                auto lhsv = (Value*)sym_entry->address;
                //FIXME(marcus): need a cleaner way to get address of variables
                //should use the symbol table instead.
                //Currently it generates a load of the lefthand side variable
                //so we can get the type!
                //NOTE(marcus): only way for something to have an address is if we alloca it?
                //Then we may need to alloca temp variables if part of an expression
                //generates a struct that we will do address of or member access on.
                auto structval = expressionCodegen(lhs,sym);
                auto structtype = structval->getType();
                //auto structtype = lhsv->getType();
                auto structmembername = ((VarNode*)rhs)->getVarName();
                //std::cout << "member name is " << structmembername << "\n";
                auto type_name = structtype->getStructName();
                auto memberlist = userTypesList.find(type_name.str());
                if(memberlist != userTypesList.end()) {
                    auto memberind = memberlist->second->find(structmembername);
                    if(memberind != memberlist->second->end()) {
                        unsigned int index = memberind->second;
                        //auto indexval = ConstantInt::get(context,APInt(32,index));
                        //std::cout << "Success, getting element index " << index << "\n";
                        //return Builder.CreateGEP(structtype,lhsv,indexval,structmembername);
                        auto memberptr = Builder.CreateStructGEP(structtype,lhsv,index,"ptr_"+structmembername);
                        if(lvalue) {
                            return memberptr;
                        }
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
                if(lvalue) {
                    return childvar;
                }
                return Builder.CreateLoad(childvar,"deref");
            } else if(op.compare("&") == 0) {
                //std::cout << "Generating address of\n";
                //TODO(marcus): we are assuming the expression underneath will be some variable
                //which has been allocated on the stack.
                if(lhs->nodeType() == AstNodeType::Var) {
                    //std::cout << "We have a var to take the address of!\n";
                    auto varname = ((VarNode*)lhs)->getVarName();
                    auto sym_entry = getFirstEntry(sym,varname);
                    return (Value*)sym_entry->address;
                }
                //TODO(marcus): this is wrong but will need to fix storing of variables for IR
                //generation first. Maybe its own symbol table?
                auto childvar = expressionCodegen(lhs,sym);
                return childvar;
            } else if(op.compare("!")) {
                //TODO(marcus): make this work for other types
                return Builder.CreateICmpEQ(lhsv,ConstantInt::get(context, APInt(32,0)),"lnottmp");
            } else {
                std::cout << "Unknown binary expression" << op << "\n";
                return val;
            }
            }
            break;
        case ANT::Const:
            {
                //std::cout << "generating constant!\n";
                auto constn = (ConstantNode*)n;
                auto strval = constn->getVal();
                if(constn->mtypeinfo.type == SemanticType::Char && constn->mtypeinfo.indirection == 1) {
                    val = Builder.CreateGlobalStringPtr(strval, "g_str");
                } else if(constn->mtypeinfo.type == SemanticType::Char) {
                    char c;
                    if(strval.size() > 3) {
                        //we have an escaped character literal
                        switch(strval[2]) {
                            case 'n':
                                c = 10;
                                break;
                            case '0':
                                c = 0;
                                break;
                            case 'a':
                                c = 7;
                                break;
                            case '\\':
                                c = 92;
                                break;
                            case '\'':
                                c = 39;
                                break;
                            case '\"':
                                c = 34;
                                break;
                            case 'r':
                                c = 13;
                                break;
                            default:
                                std::cout << "Error generating char literal " << strval << '\n';
                                c = 0;
                                break;
                        }
                        val = ConstantInt::get(context, APInt(8,c));
                    } else {
                        //we have a normal char literal
                        c = strval[1];
                        val = ConstantInt::get(context, APInt(8,c));
                    }
                } else {
                    int constval = std::stoi(strval);
                    val = ConstantInt::get(context, APInt(32,constval));
                }
            }
            break;
        case ANT::Var:
            {
                //std::cout << "generating varload!\n";
                auto varn = (VarNode*)n;
                auto sym_entry = getFirstEntry(sym, varn->getVarName());
                auto varloc = (Value*)sym_entry->address;
                if(lvalue) {
                    val = varloc;
                } else {
                    //if(varloc == nullptr) std::cout << "NULLPTR!!!\n";
                    auto varv = Builder.CreateLoad(varloc,varn->getVarName());
                    val = varv;
                }
            }
            break;
        case ANT::FuncCall:
            {
                std::cout << "generating function call";
                val = funcCallCodegen(n,sym);
            }
            break;
        case ANT::Cast:
            {
                CastNode* cast = (CastNode*)n;
                val = expressionCodegen(cast->mchildren[0], sym);
                //TODO(marcus): make sure casts work for all types
                if(cast->fromType.indirection > 0) {
                    if(cast->toType.indirection > 0) {
                        //Pointer Cast!
                        auto type = getIRType(cast->toType);
                        return Builder.CreatePointerCast(val, type, "cast");
                    }
                } else {
                    if(cast->fromType.type == SemanticType::Float || cast->fromType.type == SemanticType::floatlit) {
                        //Cast between floats
                        if(cast->toType.type == SemanticType::Float || cast->toType.type == SemanticType::Double) {
                            return Builder.CreateFPCast(val, getIRType(cast->toType), "cast");
                        }
                    }
                    if(cast->toType.type == SemanticType::Float || cast->toType.type == SemanticType::Double) {
                        //convert integer types to float/double
                        switch(cast->fromType.type) {
                            case SemanticType::u8:
                            case SemanticType::u16:
                            case SemanticType::u32:
                            case SemanticType::u64:
                                return Builder.CreateUIToFP(val, getIRType(cast->toType), "cast");
                                break;
                            case SemanticType::Int:
                            case SemanticType::intlit:
                            case SemanticType::s8:
                            case SemanticType::s16:
                            case SemanticType::s32:
                            case SemanticType::s64:
                                return Builder.CreateSIToFP(val, getIRType(cast->toType), "cast");
                                break;
                            default:
                                break;
                        }
                    }

                    switch(cast->toType.type) {
                        case SemanticType::u8:
                        case SemanticType::u16:
                        case SemanticType::u32:
                        case SemanticType::u64:
                        case SemanticType::intlit:
                        case SemanticType::Int:
                            switch(cast->fromType.type) {
                                case SemanticType::u8:
                                case SemanticType::u16:
                                case SemanticType::u32:
                                case SemanticType::u64:
                                case SemanticType::Char:
                                case SemanticType::Int:
                                    //convert unsigned to different unsigned
                                    return Builder.CreateZExtOrTrunc(val, getIRType(cast->toType), "cast");
                                    break;
                                case SemanticType::s8:
                                case SemanticType::s16:
                                case SemanticType::s32:
                                case SemanticType::s64:
                                    //TODO(marcus): implement this
                                    break;
                                default:
                                    break;
                            }
                            break;
                        case SemanticType::s8:
                        case SemanticType::s16:
                        case SemanticType::s32:
                        case SemanticType::s64:
                            switch(cast->fromType.type) {
                                case SemanticType::s8:
                                case SemanticType::s16:
                                case SemanticType::s32:
                                case SemanticType::s64:
                                    //convert signed to different signed
                                    return Builder.CreateSExtOrTrunc(val, getIRType(cast->toType), "cast");
                                    break;
                                case SemanticType::u8:
                                case SemanticType::u16:
                                case SemanticType::u32:
                                case SemanticType::u64:
                                    //TODO(marcus): implement this
                                    break;
                                default:
                                    break;
                            }
                            break;
                        default:
                            break;
                    }
                }
                //std::cout << "Implicit cast not supported from " << cast->fromType << " to " << cast->toType << '\n';
            }
        default:
            //std::cout << "default case for expression generation\n";
            break;
    }
    return val;
}
#undef ANT

void blockCodegen(AstNode* n, BasicBlock* continueto, BasicBlock* breakto, SymbolTable* sym) {
    //std::cout << "generating block\n";
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
    auto irtype = getIRType(typeinfo);
    Function* func = Builder.GetInsertBlock()->getParent();
    auto entryBB = &func->getEntryBlock();
    IRBuilder<> tmpBuilder(entryBB,entryBB->begin());
    auto alloc = tmpBuilder.CreateAlloca(irtype,0,varn->getVarName());
    symbol_table_entry->address = (void*)alloc;
    return;
}

void vardecassignCodegen(AstNode* n, SymbolTable* sym) {
    //std::cout << "Generating vardec assign\n";
    auto vardecan = (VarDecAssignNode*) n;
    auto varn = (VarNode*)vardecan->mchildren.at(0);
    auto symbol_table_entry = getFirstEntry(sym,varn->getVarName());
    //std::cout << "Looking up entry " << varn->getVarName() << '\n';
    if(!symbol_table_entry) 
    {
        std::cout << "Entry not found\n";
        std::cout << "Current Symbol Table: " << sym->name << " scope " << sym->scope <<"\n";
    }
    auto typeinfo = symbol_table_entry->typeinfo;
    //std::cout << typeinfo << '\n';
    auto ir_type = getIRType(typeinfo);
    auto func = Builder.GetInsertBlock()->getParent();
    auto entryBB = &func->getEntryBlock();
    IRBuilder<> tmpBuilder(entryBB,entryBB->begin());
    AllocaInst* alloca = tmpBuilder.CreateAlloca(ir_type,0,varn->getVarName());
    symbol_table_entry->address = alloca;
    //TODO(marcus): don't hardcode child accesses
    Value* val = expressionCodegen(vardecan->mchildren.at(1), sym);
    //std::cout << "generating store for assignment\n";
    Builder.CreateStore(val,alloca);
    return;
}

void assignCodegen(AstNode* n, SymbolTable* sym) {
    //std::cout << "Generating assingment\n";
    //TODO(marcus): don't hardcode child access
    auto assignn = (AssignNode*)n;
    auto lhs = assignn->mchildren.at(0);
    bool lval = (lhs->nodeType() != AstNodeType::Var);
    lval = true;
    auto rhs = assignn->mchildren.at(1);
    Value* val = expressionCodegen(rhs,sym);
    if(lval) {
        auto lhsv = expressionCodegen(lhs,sym,lval);
        Builder.CreateStore(val,lhsv);
    } else {
        auto varn = (VarNode*) assignn->mchildren.at(0);
        Value* var = (Value*)getFirstEntry(sym,varn->getVarName())->address;
        Builder.CreateStore(val,var);
    }
    return;
}

void ifelseCodegen(AstNode* n, BasicBlock* continueto, BasicBlock* breakto, SymbolTable* sym) {
    //std::cout << "Generating if statement\n";
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
    if(ifn->mchildren.size() == 3) {
        auto elsen = ifn->getElse();
        statementCodegen(elsen, continueto, breakto, sym);
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
    BasicBlock* endBB = BasicBlock::Create(context,"whileend");

    Builder.CreateBr(whileBB);
    Builder.SetInsertPoint(whileBB);

    auto condv = conditionalCodegen(whilen->getConditional(), sym);
    Builder.CreateCondBr(condv,beginBB,endBB);
    Builder.SetInsertPoint(beginBB);

    statementCodegen(whilen->getBody(), whileBB, endBB, sym);
    
    Builder.CreateBr(whileBB);
    enclosingscope->getBasicBlockList().push_back(endBB);
    Builder.SetInsertPoint(endBB);
    return;
}

void forloopCodegen(AstNode* n, SymbolTable* sym) {
    auto forn = (ForLoopNode*) n;
    auto scope = getScope(sym, "for"+std::to_string(forn->getId()));
    sym = scope;
    auto inits = forn->getInit();
    auto condition = forn->getConditional();
    auto update = forn->getUpdate();
    auto loopbody = forn->getBody();
    auto enclosingscope = Builder.GetInsertBlock()->getParent();
    BasicBlock* forBB = BasicBlock::Create(context,"for",enclosingscope);
    BasicBlock* beginBB = BasicBlock::Create(context,"forbegin",enclosingscope);
    BasicBlock* bodyBB = BasicBlock::Create(context,"forbody",enclosingscope);
    BasicBlock* updateBB = BasicBlock::Create(context,"forupdate");
    BasicBlock* endBB = BasicBlock::Create(context,"forend");
    
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
    enclosingscope->getBasicBlockList().push_back(updateBB);
    Builder.SetInsertPoint(updateBB);
    statementCodegen(update, updateBB, endBB, sym);
    Builder.CreateBr(beginBB);
    enclosingscope->getBasicBlockList().push_back(endBB);
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
    BasicBlock* afterBB;
    auto enclosingscope = Builder.GetInsertBlock()->getParent();
    if(std::string(n->mtoken.token).compare("break") == 0) {
        loc = breakto;
        afterBB = BasicBlock::Create(context,"afterbreak",enclosingscope);
    } else {
        loc = continueto;
        afterBB = BasicBlock::Create(context,"aftercontinue",enclosingscope);
    }

    if(loc != nullptr) {
        Builder.CreateBr(loc);
        Builder.SetInsertPoint(afterBB);
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
                {
                auto scope = getScope(sym, "while"+std::to_string(((WhileLoopNode*)n)->getId()));
                whileloopCodegen(n, scope);
                }
                break;
        case ANT::FuncCall:
                funcCallCodegen(n, sym);
                break;
        case ANT::ForLoop:
                {
                    //NOTE(marcus): gets for-loop scope in forloopCodegen
                forloopCodegen(n, sym);
                }
                break;
        case ANT::LoopStmt:
                loopstmtCodegen(n,begin,end);
                break;
        default:
            ////std::cout << "Unknown node type\n";
            //std::cout << "defaulting to expression\n";
                expressionCodegen(n,sym);
            break;
    }
}

void prepassGenerateIR(AstNode* ast, SymbolTable* sym) {
    //check for null
    if(!ast)
        return;

    //Handle IR gen for each node type
    switch(ast->nodeType()) {
        case ANT::Prototype:
            {
                auto scope = getScope(sym, ((FuncDefNode*)ast)->mfuncname);
                prototypeCodegen(ast, scope);
                return;
            }
            break;
        case ANT::FuncDef:
            {
                auto scope = getScope(sym, ((FuncDefNode*)ast)->mfuncname + std::to_string(((FuncDefNode*)ast)->id));
                functionCodegen(ast, scope, true);
               return;
            }
            break;
        case ANT::CompileUnit:
            {
                auto scope = getScope(sym, ((CompileUnitNode*)ast)->getFileName());
                std::vector<AstNode*>* vec = ast->getChildren();
                for(auto c : (*vec)) {
                    prepassGenerateIR(c,scope);
                }
                return;
            }
            break;
        case ANT::Program:
            {
                std::vector<AstNode*>* vec = ast->getChildren();
                for(auto c : (*vec)) {
                    prepassGenerateIR(c,sym);
                }
                return;
            }
            break;
        default:
            break;
    }

    //recurse
    std::vector<AstNode*>* vec = ast->getChildren();
    for(auto c : (*vec)) {
        prepassGenerateIR(c, sym);
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
                prototypeCodegen(ast, scope);
                return;
            }
            break;
        case ANT::FuncDef:
            {
                std::cout << "Generating FuncDef IR\n";
                auto scope = getScope(sym, ((FuncDefNode*)ast)->mfuncname + std::to_string(((FuncDefNode*)ast)->id));
                functionCodegen(ast, scope);
               statementCodegen(((FuncDefNode*)ast)->getFunctionBody(),nullptr,nullptr,scope);
               //verifyFunction(*F);
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
    prepassGenerateIR(ast,&progSymTab);
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
    //std::cout << "OPTIMIZED IR.....\n";
    //dumpIR();
    dest.flush();
    //std::cout << "wrote out object file\n";

    //outs() << "Wrote " << out << "\n";
    return;
}

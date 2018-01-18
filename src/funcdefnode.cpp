#include "funcdefnode.h"
#include "astnodetypes.h"
#include <iostream>

int FuncDefNode::count = 0;

FuncDefNode::FuncDefNode(AstNodeType ntype) {
    id = FuncDefNode::count;
    FuncDefNode::count++;
    mchildren.reserve(8);
    isOperatorOverload = 0;
    op = nullptr;
    mnodet = ntype;
}

FuncDefNode::~FuncDefNode() {
}

void FuncDefNode::addFuncName(std::string funcname) {
    mfuncname = funcname;
}

ArrayView FuncDefNode::getParameters() {
    auto s = mchildren.size();
    if(mnodet == AstNodeType::FuncDef) s -= 1;
    auto p = s ? &mchildren[0] : nullptr;
    ArrayView params(p,s);
    return params;
}

AstNode* FuncDefNode::getFunctionBody() {
    return mchildren.back();
}

/*
enum class SemanticType {
    Bool,
    Int,
    Void,
    Char,
    Double,
    Float,
    Typeless,
    Infer,
    User,
    u8,
    u16,
    u32,
    u64,
    s8,
    s16,
    s32,
    s64,
    intlit,
    floatlit
*/
std::string FuncDefNode::mangledName() {
    if(mfuncname == "main") {
        return mfuncname;
    }

    std::string mangled = mfuncname + "_";
    for(auto arg : mchildren) {
        if(arg->nodeType() != AstNodeType::Params)
            continue;
        auto argti = arg->mtypeinfo;
        if(argti.indirection) {
            mangled = mangled + "p" + std::to_string(argti.indirection);
        }
        switch(argti.type) {
            case SemanticType::Void:
                mangled += "v";
                break;
            case SemanticType::Bool:
                mangled += "b";
                break;
            case SemanticType::Char:
                mangled += "c";
                break;
            case SemanticType::Int:
                mangled += "i";
                break;
            case SemanticType::Float:
                mangled += "f";
                break;
            case SemanticType::Double:
                mangled += "d";
                break;
            case SemanticType::u8:
                mangled += "u8";
                break;
            case SemanticType::u32:
                mangled += "u32";
                break;
            case SemanticType::u64:
                mangled += "u64";
                break;
            case SemanticType::s8:
                mangled += "s8";
                break;
            case SemanticType::s32:
                mangled += "s32";
                break;
            case SemanticType::s64:
                mangled += "s64";
                break;
            case SemanticType::User:
                mangled += argti.userid;
                break;
            default:
                mangled += "_default_";
                break;
        }
    }
    //std::cout << "Mangled: " << mangled << '\n';
    return mangled;
}

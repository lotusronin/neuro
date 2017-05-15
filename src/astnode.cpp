#include "astnode.h"
#include <iostream>

AstNode::AstNode() {
}

AstNode::~AstNode() {
}

AstNodeType AstNode::nodeType() {
    return AstNodeType::Default;
}

SemanticType AstNode::getType() {
    return mtypeinfo.type;
}

void AstNode::addChild(AstNode* child) {
    //std::cout << "addChild not called on subclass\n";
    mchildren.push_back(child);
}

AstNode* AstNode::lastChild() {
    //std::cout << "lastChild not called on subclass\n";
    return mchildren.back();
}

std::vector<AstNode*>* AstNode::getChildren() {
    //std::cout << "getChildren not called on subclass\n";
    return &mchildren;
}

void AstNode::setToken(Token& t) {
    mtoken = t;
}

TypeInfo* AstNode::getTypeInfo() {
    return &mtypeinfo;
}

std::ostream& operator<<(std::ostream& os, const SemanticType& obj) {
    const char* out;
    switch(obj) {
        case SemanticType::Bool:
            out = "bool";
            break;
        case SemanticType::Int:
            out = "int";
            break;
        case SemanticType::Void:
            out = "void";
            break;
        case SemanticType::Char:
            out = "char";
            break;
        case SemanticType::Double:
            out = "double";
            break;
        case SemanticType::Float:
            out = "float";
            break;
        case SemanticType::Typeless:
            out = "typeless";
            break;
        case SemanticType::Infer:
            out = "Inferred";
            break;
        case SemanticType::intlit:
            out = "intlit";
            break;
        case SemanticType::User:
            out = "";
            break;
        case SemanticType::u32:
            out = "u32";
            break;
        default:
            out = "Unrecognized value!";
            break;
    }
    os << out;
    return os;
}

std::ostream& operator<<(std::ostream& os, const TypeInfo& obj) {
    if(obj.indirection > 0) {
        int i = obj.indirection;
        while(i) {
            os << '*';
            --i;
        }
    }
    os << obj.type;
    if(obj.userid.size() > 0) {
        os << obj.userid;
    }
    return os;
}

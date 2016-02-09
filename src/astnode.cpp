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
    return mstype;
}

void AstNode::makeGraph(std::ofstream& outfile) {
    std::cout << "makeGraph not called on subclass\n";
}

void AstNode::addChild(AstNode* child) {
    std::cout << "addChild not called on subclass\n";
}

AstNode* AstNode::lastChild() {
    std::cout << "lastChild not called on subclass\n";
    return nullptr;
}

std::vector<AstNode*>* AstNode::getChildren() {
    std::cout << "getChildren not called on subclass\n";
    return nullptr;
}

void AstNode::setToken(Token& t) {
    mtoken = t;
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
            out = "";
            break;
        default:
            out = "Unrecognized value!";
            break;
    }
    os << out;
    return os;
}

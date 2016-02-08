#include "astnode.h"
#include <iostream>

AstNode::AstNode() {
}

AstNode::~AstNode() {
}

AstNodeType AstNode::nodeType() {
    return AstNodeType::Default;
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

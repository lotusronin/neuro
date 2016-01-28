#include "astnode.h"
#include <iostream>

AstNode::AstNode() {
}

AstNode::~AstNode() {
}

AstNodeType AstNode::type() {
    return AstNodeType::Default;
}

void AstNode::makeGraph(std::ofstream& outfile) {
    std::cout << "makeGraph not called on subclass\n";
}

void AstNode::addChild(AstNode* child) {
    std::cout << "addChild not called on subclass\n";
}

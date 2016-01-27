#include "astnode.h"
#include <iostream>

AstNode::AstNode() {
}

AstNode::~AstNode() {
}

AstNodeType AstNode::type() {
    return AstNodeType::Default;
}

void AstNode::makeGraph() {
    std::cout << "makeGraph not called on subclass\n";
}

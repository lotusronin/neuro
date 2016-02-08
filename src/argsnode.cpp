#include "argsnode.h"
#include <iostream>

ArgsNode::ArgsNode() {
}

ArgsNode::~ArgsNode() {
}

void ArgsNode::makeGraph() {
    //implement this
}

AstNodeType ArgsNode::nodeType() {
    return AstNodeType::Args;
}

void ArgsNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

std::vector<AstNode*>* ArgsNode::getChildren() {
    return &mchildren;
}

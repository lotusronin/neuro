#include "argsnode.h"
#include <iostream>

ArgsNode::ArgsNode() {
}

ArgsNode::~ArgsNode() {
}

void ArgsNode::makeGraph() {
    //implement this
}

AstNodeType ArgsNode::type() {
    return AstNodeType::Args;
}

void ArgsNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

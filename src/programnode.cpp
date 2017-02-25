#include "programnode.h"
#include <iostream>

ProgramNode::ProgramNode() {
}

ProgramNode::~ProgramNode() {
}

AstNodeType ProgramNode::nodeType() {
    return AstNodeType::Program;
}

void ProgramNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

std::vector<AstNode*>* ProgramNode::getChildren() {
    return &mchildren;
}

#include "programnode.h"

ProgramNode::ProgramNode() {
    mnodet = AstNodeType::Program;
}

ProgramNode::~ProgramNode() {
}

void ProgramNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

std::vector<AstNode*>* ProgramNode::getChildren() {
    return &mchildren;
}

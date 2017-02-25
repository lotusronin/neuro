#include "funccallnode.h"
#include <iostream>

int FuncCallNode::count = 0;

FuncCallNode::FuncCallNode() {
    id = FuncCallNode::count;
    FuncCallNode::count++;
}

FuncCallNode::~FuncCallNode() {
}

AstNodeType FuncCallNode::nodeType() {
    return AstNodeType::FuncCall;
}

void FuncCallNode::addArgs(AstNode* node) {
    margs.push_back(node);
}

void FuncCallNode::addChild(AstNode* node) {
    margs.push_back(node);
}

void FuncCallNode::addFuncName(std::string funcname) {
    mfuncname = funcname;
}

std::vector<AstNode*>* FuncCallNode::getChildren() {
    return &margs;
}

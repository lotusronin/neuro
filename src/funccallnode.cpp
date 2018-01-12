#include "funccallnode.h"

int FuncCallNode::count = 0;

FuncCallNode::FuncCallNode() {
    id = FuncCallNode::count;
    FuncCallNode::count++;
    mnodet = AstNodeType::FuncCall;
}

FuncCallNode::~FuncCallNode() {
}

void FuncCallNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

void FuncCallNode::addFuncName(std::string funcname) {
    mfuncname = funcname;
}

std::vector<AstNode*>* FuncCallNode::getChildren() {
    return &mchildren;
}

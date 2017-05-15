#include "loopstmtnode.h"
#include <iostream>

int LoopStmtNode::count = 0;

LoopStmtNode::LoopStmtNode() {
    id = LoopStmtNode::count;
    LoopStmtNode::count++;
}

LoopStmtNode::~LoopStmtNode() {
}

AstNodeType LoopStmtNode::nodeType() {
    return AstNodeType::LoopStmt;
}

void LoopStmtNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

void LoopStmtNode::setBreak(bool b) {
    isBreak = b;
}

std::vector<AstNode*>* LoopStmtNode::getChildren() {
    return &mchildren;
}

#include "deferstmtnode.h"
#include <iostream>

int DeferStmtNode::count = 0;

DeferStmtNode::DeferStmtNode() {
    id = DeferStmtNode::count;
    DeferStmtNode::count++;
}

DeferStmtNode::~DeferStmtNode() {
}

AstNodeType DeferStmtNode::nodeType() {
    return AstNodeType::DeferStmt;
}

void DeferStmtNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

std::vector<AstNode*>* DeferStmtNode::getChildren() {
    return &mchildren;
}

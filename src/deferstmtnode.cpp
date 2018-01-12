#include "deferstmtnode.h"

int DeferStmtNode::count = 0;

DeferStmtNode::DeferStmtNode() {
    id = DeferStmtNode::count;
    DeferStmtNode::count++;
    mnodet = AstNodeType::DeferStmt;
}

DeferStmtNode::~DeferStmtNode() {
}

void DeferStmtNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

std::vector<AstNode*>* DeferStmtNode::getChildren() {
    return &mchildren;
}

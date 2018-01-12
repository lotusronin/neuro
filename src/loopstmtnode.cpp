#include "loopstmtnode.h"

int LoopStmtNode::count = 0;

LoopStmtNode::LoopStmtNode() {
    id = LoopStmtNode::count;
    LoopStmtNode::count++;
    mnodet = AstNodeType::LoopStmt;
    isBreak = false;
}

LoopStmtNode::~LoopStmtNode() {
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

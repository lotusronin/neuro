#include "loopstmtnode.h"

int LoopStmtNode::count = 0;

LoopStmtNode::LoopStmtNode() {
    id = LoopStmtNode::count;
    LoopStmtNode::count++;
    mnodet = AstNodeType::LoopStmtBrk;
}

LoopStmtNode::LoopStmtNode(LoopStmtNode* n) {
    id = LoopStmtNode::count;
    LoopStmtNode::count++;
    mnodet = n->mnodet;
    mtoken = n->mtoken;
}

LoopStmtNode::~LoopStmtNode() {
}

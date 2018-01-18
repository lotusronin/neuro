#include "deferstmtnode.h"

int DeferStmtNode::count = 0;

DeferStmtNode::DeferStmtNode() {
    id = DeferStmtNode::count;
    DeferStmtNode::count++;
    mnodet = AstNodeType::DeferStmt;
}

DeferStmtNode::~DeferStmtNode() {
}

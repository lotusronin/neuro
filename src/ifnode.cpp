#include "ifnode.h"

int IfNode::count = 0;

IfNode::IfNode() {
    id = IfNode::count;
    IfNode::count++;
    mnodet = AstNodeType::IfStmt;
}

IfNode::IfNode(IfNode* n) {
    id = IfNode::count;
    IfNode::count++;
    mnodet = AstNodeType::IfStmt;
    mtoken = n->mtoken;
}

IfNode::~IfNode() {
}

AstNode* IfNode::getConditional() {
    return mchildren.at(0);
}

AstNode* IfNode::getThen() {
    return mchildren.at(1);
}

AstNode* IfNode::getElse() {
    bool hasElse = mchildren.size() == 3;
    return hasElse ? mchildren[2] : nullptr;
}

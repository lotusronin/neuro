#include "assignnode.h"

int AssignNode::count = 0;

AssignNode::AssignNode() {
    id = AssignNode::count;
    AssignNode::count++;
    mnodet = AstNodeType::Assign;
}

AssignNode::AssignNode(AssignNode* n) {
    id = AssignNode::count;
    AssignNode::count++;
    mnodet = AstNodeType::Assign;
    mtoken = n->mtoken;
}

AssignNode::~AssignNode() {
}

void AssignNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

AstNode* AssignNode::getLHS() {
    return mchildren.at(0);
}

AstNode* AssignNode::getRHS() {
    return mchildren.at(1);
}

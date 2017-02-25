#include "assignnode.h"
#include <iostream>

int AssignNode::count = 0;

AssignNode::AssignNode() {
    id = AssignNode::count;
    AssignNode::count++;
}

AssignNode::~AssignNode() {
}

AstNodeType AssignNode::nodeType() {
    return AstNodeType::Assign;
}

void AssignNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

std::vector<AstNode*>* AssignNode::getChildren() {
    return &mchildren;
}

AstNode* AssignNode::getLHS() {
    return mchildren.at(0);
}

AstNode* AssignNode::getRHS() {
    return mchildren.at(1);
}

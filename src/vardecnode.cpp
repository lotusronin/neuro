#include "vardecnode.h"
#include <iostream>

int VarDecNode::count = 0;

VarDecNode::VarDecNode() {
    id = VarDecNode::count;
    VarDecNode::count++;
    mchildren.reserve(1);
}

VarDecNode::~VarDecNode() {
}

AstNodeType VarDecNode::nodeType() {
    return AstNodeType::VarDec;
}

void VarDecNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

std::vector<AstNode*>* VarDecNode::getChildren() {
    return &mchildren;
}

AstNode* VarDecNode::getLHS() {
    return mchildren.at(0);
}

AstNode* VarDecNode::getRHS() {
    return mchildren.at(1);
}

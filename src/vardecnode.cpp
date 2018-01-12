#include "vardecnode.h"

int VarDecNode::count = 0;

VarDecNode::VarDecNode() {
    id = VarDecNode::count;
    VarDecNode::count++;
    mchildren.reserve(1);
    mnodet = AstNodeType::VarDec;
}

VarDecNode::~VarDecNode() {
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

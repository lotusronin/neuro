#include "vardecassignnode.h"

int VarDecAssignNode::count = 0;

VarDecAssignNode::VarDecAssignNode() {
    id = VarDecAssignNode::count;
    VarDecAssignNode::count++;
    mchildren.reserve(2);
    mnodet = AstNodeType::VarDecAssign;
}

VarDecAssignNode::~VarDecAssignNode() {
}

void VarDecAssignNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

std::vector<AstNode*>* VarDecAssignNode::getChildren() {
    return &mchildren;
}

AstNode* VarDecAssignNode::getLHS() {
    return mchildren.at(0);
}

AstNode* VarDecAssignNode::getRHS() {
    return mchildren.at(1);
}

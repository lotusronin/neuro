#include "castnode.h"
#include <iostream>

int CastNode::count = 0;

CastNode::CastNode() {
    id = CastNode::count;
    CastNode::count++;
}

CastNode::~CastNode() {
}

AstNodeType CastNode::nodeType() {
    return AstNodeType::Cast;
}

void CastNode::addChild(AstNode* n) {
    //TODO(marcus): should we allow more than 1 child for Cast nodes?
    mchildren.push_back(n);
}

std::vector<AstNode*>* CastNode::getChildren() {
    return &mchildren;
}

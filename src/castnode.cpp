#include "castnode.h"

int CastNode::count = 0;

CastNode::CastNode() {
    id = CastNode::count;
    CastNode::count++;
    mnodet = AstNodeType::Cast;
}

CastNode::~CastNode() {
}

void CastNode::addChild(AstNode* n) {
    //TODO(marcus): should we allow more than 1 child for Cast nodes?
    mchildren.push_back(n);
}

std::vector<AstNode*>* CastNode::getChildren() {
    return &mchildren;
}

#include "sizeofnode.h"

int SizeOfNode::count = 0;

SizeOfNode::SizeOfNode() {
    id = SizeOfNode::count;
    SizeOfNode::count++;
}

SizeOfNode::~SizeOfNode() {
}

AstNodeType SizeOfNode::nodeType() {
    return AstNodeType::SizeOf;
}

void SizeOfNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

std::vector<AstNode*>* SizeOfNode::getChildren() {
    return &mchildren;
}

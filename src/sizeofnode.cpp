#include "sizeofnode.h"

int SizeOfNode::count = 0;

SizeOfNode::SizeOfNode() {
    id = SizeOfNode::count;
    SizeOfNode::count++;
    mnodet = AstNodeType::SizeOf;
}

SizeOfNode::~SizeOfNode() {
}

void SizeOfNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

std::vector<AstNode*>* SizeOfNode::getChildren() {
    return &mchildren;
}

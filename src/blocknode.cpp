#include "blocknode.h"

int BlockNode::count = 0;

BlockNode::BlockNode() {
    id = BlockNode::count;
    BlockNode::count++;
    mnodet = AstNodeType::Block;
}

BlockNode::~BlockNode() {
}

void BlockNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

AstNode* BlockNode::lastChild() {
    return mchildren.back();
}

std::vector<AstNode*>* BlockNode::getChildren() {
    return &mchildren;
}

int BlockNode::getId() {
    return id;
}

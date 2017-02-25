#include "blocknode.h"
#include <iostream>

int BlockNode::count = 0;

BlockNode::BlockNode() {
    id = BlockNode::count;
    BlockNode::count++;
}

BlockNode::~BlockNode() {
}

AstNodeType BlockNode::nodeType() {
    return AstNodeType::Block;
}

void BlockNode::addChild(AstNode* node) {
    mstatements.push_back(node);
}

AstNode* BlockNode::lastChild() {
    return mstatements.back();
}

std::vector<AstNode*>* BlockNode::getChildren() {
    return &mstatements;
}

int BlockNode::getId() {
    return id;
}

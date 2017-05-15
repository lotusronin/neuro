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
    //mstatements.push_back(node);
    mchildren.push_back(node);
}

AstNode* BlockNode::lastChild() {
    //return mstatements.back();
    return mchildren.back();
}

std::vector<AstNode*>* BlockNode::getChildren() {
    //return &mstatements;
    return &mchildren;
}

int BlockNode::getId() {
    return id;
}

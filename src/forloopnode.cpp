#include "forloopnode.h"
#include <iostream>

int ForLoopNode::count = 0;

ForLoopNode::ForLoopNode() {
    id = ForLoopNode::count;
    ForLoopNode::count++;
}

ForLoopNode::~ForLoopNode() {
}

AstNodeType ForLoopNode::nodeType() {
    return AstNodeType::ForLoop;
}

void ForLoopNode::addChild(AstNode* node) {
    mstatements.push_back(node);
}

std::vector<AstNode*>* ForLoopNode::getChildren() {
    return &mstatements;
}

AstNode* ForLoopNode::getInit() {
    return mstatements.at(0);
}

AstNode* ForLoopNode::getConditional() {
    return mstatements.at(1);
}

AstNode* ForLoopNode::getUpdate() {
    return mstatements.at(2);
}

AstNode* ForLoopNode::getBody() {
    return mstatements.at(3);
}

int ForLoopNode::getId() {
    return id;
}

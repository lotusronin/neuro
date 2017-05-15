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
    mchildren.push_back(node);
}

std::vector<AstNode*>* ForLoopNode::getChildren() {
    return &mchildren;
}

AstNode* ForLoopNode::getInit() {
    return mchildren.at(0);
}

AstNode* ForLoopNode::getConditional() {
    return mchildren.at(1);
}

AstNode* ForLoopNode::getUpdate() {
    return mchildren.at(2);
}

AstNode* ForLoopNode::getBody() {
    return mchildren.at(3);
}

int ForLoopNode::getId() {
    return id;
}

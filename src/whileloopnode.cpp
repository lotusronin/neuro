#include "whileloopnode.h"
#include <iostream>

int WhileLoopNode::count = 0;

WhileLoopNode::WhileLoopNode() {
    id = WhileLoopNode::count;
    WhileLoopNode::count++;
}

WhileLoopNode::~WhileLoopNode() {
}

AstNodeType WhileLoopNode::nodeType() {
    return AstNodeType::WhileLoop;
}

void WhileLoopNode::addChild(AstNode* node) {
    mstatements.push_back(node);
}

std::vector<AstNode*>* WhileLoopNode::getChildren() {
    return &mstatements;
}

AstNode* WhileLoopNode::getConditional() {
    return mstatements.at(0);
}

AstNode* WhileLoopNode::getBody() {
    return mstatements.at(1);
}

int WhileLoopNode::getId() {
    return id;
}

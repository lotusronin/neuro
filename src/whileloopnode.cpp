#include "whileloopnode.h"

int WhileLoopNode::count = 0;

WhileLoopNode::WhileLoopNode() {
    id = WhileLoopNode::count;
    WhileLoopNode::count++;
    mnodet = AstNodeType::WhileLoop;
}

WhileLoopNode::~WhileLoopNode() {
}

void WhileLoopNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

std::vector<AstNode*>* WhileLoopNode::getChildren() {
    return &mchildren;
}

AstNode* WhileLoopNode::getConditional() {
    return mchildren.at(0);
}

AstNode* WhileLoopNode::getBody() {
    return mchildren.at(1);
}

int WhileLoopNode::getId() {
    return id;
}

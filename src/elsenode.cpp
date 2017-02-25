#include "elsenode.h"
#include <iostream>

int ElseNode::count = 0;

ElseNode::ElseNode() {
    id = ElseNode::count;
    ElseNode::count++;
}

ElseNode::~ElseNode() {
}

AstNodeType ElseNode::nodeType() {
    return AstNodeType::ElseStmt;
}

void ElseNode::addChild(AstNode* node) {
    mstatements.push_back(node);
}

std::vector<AstNode*>* ElseNode::getChildren() {
    return &mstatements;
}

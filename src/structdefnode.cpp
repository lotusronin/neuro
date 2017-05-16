#include "structdefnode.h"
#include <iostream>

int StructDefNode::count = 0;

StructDefNode::StructDefNode() {
    id = StructDefNode::count;
    StructDefNode::count++;
    foreign = false;
}

StructDefNode::~StructDefNode() {
}

AstNodeType StructDefNode::nodeType() {
    return AstNodeType::StructDef;
}

void StructDefNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

std::vector<AstNode*>* StructDefNode::getChildren() {
    return &mchildren;
}

std::string& StructDefNode::getIdent() {
    return ident;
}

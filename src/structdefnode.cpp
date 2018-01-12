#include "structdefnode.h"

int StructDefNode::count = 0;

StructDefNode::StructDefNode() {
    id = StructDefNode::count;
    StructDefNode::count++;
    foreign = false;
    mnodet = AstNodeType::StructDef;
}

StructDefNode::~StructDefNode() {
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

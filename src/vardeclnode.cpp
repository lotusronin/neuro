#include "vardeclnode.h"
#include <cassert>

int VarDeclNode::count = 0;

VarDeclNode::VarDeclNode(AstNodeType ntype) {
    id = VarDeclNode::count;
    VarDeclNode::count++;
    mchildren.reserve(2);
    mnodet = ntype;
}

VarDeclNode::~VarDeclNode() {
}

void VarDeclNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

AstNode* VarDeclNode::getLHS() {
    return mchildren.at(0);
}

AstNode* VarDeclNode::getRHS() {
    assert(mnodet == AstNodeType::VarDecAssign);
    return mchildren.at(1);
}

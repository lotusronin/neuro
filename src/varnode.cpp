#include "varnode.h"

int VarNode::count = 0;

VarNode::VarNode() {
    id = VarNode::count;
    VarNode::count++;
    mnodet = AstNodeType::Var;
}

VarNode::~VarNode() {
}

void VarNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

void VarNode::addVarName(std::string name) {
    mname = name;
}

std::string VarNode::getVarName() {
    return mname;
}

std::vector<AstNode*>* VarNode::getChildren() {
    return &mchildren;
}

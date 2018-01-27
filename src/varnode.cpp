#include "varnode.h"

int VarNode::count = 0;

VarNode::VarNode() {
    id = VarNode::count;
    VarNode::count++;
    mnodet = AstNodeType::Var;
}

VarNode::~VarNode() {
}

void VarNode::addVarName(std::string name) {
    mname = name;
}

const char* VarNode::getVarName() const {
    return mname.c_str();
}

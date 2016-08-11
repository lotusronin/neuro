#include "vardecassignnode.h"
#include <iostream>

int VarDecAssignNode::count = 0;

VarDecAssignNode::VarDecAssignNode() {
    id = VarDecAssignNode::count;
    VarDecAssignNode::count++;
}

VarDecAssignNode::~VarDecAssignNode() {
}

void VarDecAssignNode::makeGraph(std::ofstream& outfile) {
    outfile << "vardecassign" << id << ";\n";
    outfile << "vardecassign" << id << "[label=\"vardecassign\"];\n";
    for (auto child : mchildren) {
        outfile << "vardecassign" << id << " -> ";
        child->makeGraph(outfile);
    }
}

AstNodeType VarDecAssignNode::nodeType() {
    return AstNodeType::VarDecAssign;
}

void VarDecAssignNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

std::vector<AstNode*>* VarDecAssignNode::getChildren() {
    return &mchildren;
}

AstNode* VarDecAssignNode::getLHS() {
    return mchildren.at(0);
}

AstNode* VarDecAssignNode::getRHS() {
    return mchildren.at(1);
}

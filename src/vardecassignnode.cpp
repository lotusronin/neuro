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
    for (auto param : mparams) {
        outfile << "vardecassign" << id << " -> ";
        param->makeGraph(outfile);
    }
}

AstNodeType VarDecAssignNode::type() {
    return AstNodeType::VarDecAssign;
}

void VarDecAssignNode::addChild(AstNode* node) {
    mstatements.push_back(node);
}


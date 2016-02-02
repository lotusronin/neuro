#include "vardecnode.h"
#include <iostream>

int VarDecNode::count = 0;

VarDecNode::VarDecNode() {
    id = VarDecNode::count;
    VarDecNode::count++;
}

VarDecNode::~VarDecNode() {
}

void VarDecNode::makeGraph(std::ofstream& outfile) {
    outfile << "vardec" << id << ";\n";
    outfile << "vardec" << id << "[label=\"vardec\"];\n";
    for (auto child : mchildren) {
        outfile << "vardec" << id << " -> ";
        child->makeGraph(outfile);
    }
}

AstNodeType VarDecNode::type() {
    return AstNodeType::VarDec;
}

void VarDecNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

std::vector<AstNode*>* VarDecNode::getChildren() {
    return &mchildren;
}

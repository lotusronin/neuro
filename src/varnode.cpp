#include "varnode.h"
#include <iostream>

int VarNode::count = 0;

VarNode::VarNode() {
    id = VarNode::count;
    VarNode::count++;
}

VarNode::~VarNode() {
}

void VarNode::makeGraph(std::ofstream& outfile) {
    //implement this
    outfile << "var"<<id<<";\n";
    outfile << "var"<<id<<"[label=\""<<mname<<"\"];\n";
    for (auto child : mchildren) {
        outfile << "var" << id << " -> ";
        child->makeGraph(outfile);
    }
}

AstNodeType VarNode::type() {
    return AstNodeType::Var;
}

void VarNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

void VarNode::addVarName(std::string name) {
    mname = name;
}

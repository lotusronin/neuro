#include "programnode.h"
#include <iostream>

ProgramNode::ProgramNode() {
}

ProgramNode::~ProgramNode() {
}

void ProgramNode::makeGraph(std::ofstream& outfile) {
    outfile << "digraph Program {\n";
    outfile << "Prog [label=\"Program\"];\n";
    for (auto child : mchildren) {
        outfile << "Prog -> ";
        child->makeGraph(outfile);
    }
    outfile << "}";
}

AstNodeType ProgramNode::nodeType() {
    return AstNodeType::Program;
}

void ProgramNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

std::vector<AstNode*>* ProgramNode::getChildren() {
    return &mchildren;
}

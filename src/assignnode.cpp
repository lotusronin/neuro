#include "assignnode.h"
#include <iostream>

int AssignNode::count = 0;

AssignNode::AssignNode() {
    id = AssignNode::count;
    AssignNode::count++;
}

AssignNode::~AssignNode() {
}

void AssignNode::makeGraph(std::ofstream& outfile) {
    outfile << "assign" << id << ";\n";
    outfile << "assign" << id << "[label=\"=\"];\n";
    for (auto s : mchildren) {
        outfile << "assign" << id << " -> ";
        s->makeGraph(outfile);
    }
}

AstNodeType AssignNode::type() {
    return AstNodeType::Assign;
}

void AssignNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}


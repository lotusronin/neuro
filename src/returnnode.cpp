#include "returnnode.h"
#include <iostream>

int ReturnNode::count = 0;

ReturnNode::ReturnNode() {
    id = ReturnNode::count;
    ReturnNode::count++;
}

ReturnNode::~ReturnNode() {
}

void ReturnNode::makeGraph(std::ofstream& outfile) {
    outfile << "return" << id << ";\n";
    outfile << "return" << id << "[label=\"return\"];\n";
    for (auto child : mchildren) {
        outfile << "return" << id << " -> ";
        child->makeGraph(outfile);
    }
}

AstNodeType ReturnNode::type() {
    return AstNodeType::RetStmnt;
}

void ReturnNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

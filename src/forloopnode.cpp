#include "forloopnode.h"
#include <iostream>

int ForLoopNode::count = 0;

ForLoopNode::ForLoopNode() {
    id = ForLoopNode::count;
    ForLoopNode::count++;
}

ForLoopNode::~ForLoopNode() {
}

void ForLoopNode::makeGraph(std::ofstream& outfile) {
    outfile << "forloop" << id << ";\n";
    outfile << "forloop" << id << "[label=\"for\"];\n";
    for (auto s : mstatements) {
        outfile << "forloop" << id << " -> ";
        s->makeGraph(outfile);
    }
}

AstNodeType ForLoopNode::nodeType() {
    return AstNodeType::ForLoop;
}

void ForLoopNode::addChild(AstNode* node) {
    mstatements.push_back(node);
}

std::vector<AstNode*>* ForLoopNode::getChildren() {
    return &mstatements;
}

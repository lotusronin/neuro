#include "whileloopnode.h"
#include <iostream>

int WhileLoopNode::count = 0;

WhileLoopNode::WhileLoopNode() {
    id = WhileLoopNode::count;
    WhileLoopNode::count++;
}

WhileLoopNode::~WhileLoopNode() {
}

void WhileLoopNode::makeGraph(std::ofstream& outfile) {
    outfile << "whileloop" << id << ";\n";
    outfile << "whileloop" << id << "[label=\"while\"];\n";
    for (auto s : mstatements) {
        outfile << "whileloop" << id << " -> ";
        s->makeGraph(outfile);
    }
}

AstNodeType WhileLoopNode::nodeType() {
    return AstNodeType::WhileLoop;
}

void WhileLoopNode::addChild(AstNode* node) {
    mstatements.push_back(node);
}

std::vector<AstNode*>* WhileLoopNode::getChildren() {
    return &mstatements;
}

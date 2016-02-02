#include "ifnode.h"
#include <iostream>

int IfNode::count = 0;

IfNode::IfNode() {
    id = IfNode::count;
    IfNode::count++;
}

IfNode::~IfNode() {
}

void IfNode::makeGraph(std::ofstream& outfile) {
    outfile << "if" << id << ";\n";
    outfile << "if" << id << "[label=\"if\"];\n";
    for (auto s : mstatements) {
        outfile << "if" << id << " -> ";
        s->makeGraph(outfile);
    }
}

AstNodeType IfNode::type() {
    return AstNodeType::IfStmt;
}

void IfNode::addChild(AstNode* node) {
    mstatements.push_back(node);
}

std::vector<AstNode*>* IfNode::getChildren() {
    return &mstatements;
}

#include "elsenode.h"
#include <iostream>

int ElseNode::count = 0;

ElseNode::ElseNode() {
    id = ElseNode::count;
    ElseNode::count++;
}

ElseNode::~ElseNode() {
}

void ElseNode::makeGraph(std::ofstream& outfile) {
    outfile << "else" << id << ";\n";
    outfile << "else" << id << "[label=\"else\"];\n";
    for (auto s : mstatements) {
        outfile << "else" << id << " -> ";
        s->makeGraph(outfile);
    }
}

AstNodeType ElseNode::type() {
    return AstNodeType::ElseStmt;
}

void ElseNode::addChild(AstNode* node) {
    mstatements.push_back(node);
}


#include "deferstmtnode.h"
#include <iostream>

int DeferStmtNode::count = 0;

DeferStmtNode::DeferStmtNode() {
    id = DeferStmtNode::count;
    DeferStmtNode::count++;
}

DeferStmtNode::~DeferStmtNode() {
}

void DeferStmtNode::makeGraph(std::ofstream& outfile) {
    //implement this
    outfile << "defer"<<id<<";\n";
    outfile << "defer"<<id<<"[label=\"defer\"];\n";
    for (auto child : mchildren) {
        outfile << "defer" << id << " -> ";
        child->makeGraph(outfile);
    }
}

AstNodeType DeferStmtNode::type() {
    return AstNodeType::DeferStmt;
}

void DeferStmtNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

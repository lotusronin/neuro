#include "loopstmtnode.h"
#include <iostream>

int LoopStmtNode::count = 0;

LoopStmtNode::LoopStmtNode() {
    id = LoopStmtNode::count;
    LoopStmtNode::count++;
}

LoopStmtNode::~LoopStmtNode() {
}

void LoopStmtNode::makeGraph(std::ofstream& outfile) {
    //implement this
    outfile << "loopStmt" << id << ";\n";
    if(isBreak) {
        outfile << "loopStmt" << id << "[label=\"break\"];\n";
    } else {
        outfile << "loopStmt" << id << "[label=\"continue\"];\n";
    }
    for (auto child : mchildren) {
        outfile << "loopStmt" << id << " -> ";
        child->makeGraph(outfile);
    }
}

AstNodeType LoopStmtNode::type() {
    return AstNodeType::LoopStmt;
}

void LoopStmtNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

void LoopStmtNode::setBreak(bool b) {
    isBreak = b;
}

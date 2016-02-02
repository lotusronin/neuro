#include "binopnode.h"
#include <iostream>

int BinOpNode::count = 0;

BinOpNode::BinOpNode() {
    id = BinOpNode::count;
    BinOpNode::count++;
}

BinOpNode::~BinOpNode() {
}

void BinOpNode::makeGraph(std::ofstream& outfile) {
    outfile << "binop" << id << ";\n";
    outfile << "binop" << id << "[label=\"" << mop << "\"];\n";
    for (auto child : mchildren) {
        outfile << "binop" << id << " -> ";
        child->makeGraph(outfile);
    }
}

AstNodeType BinOpNode::type() {
    return AstNodeType::BinOp;
}

void BinOpNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

void BinOpNode::setOp(std::string& op) {
    mop = op;
}

std::string BinOpNode::getOp() {
    return mop;
}

std::vector<AstNode*>* BinOpNode::getChildren() {
    return &mchildren;
}

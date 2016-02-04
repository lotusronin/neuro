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

AstNode* BinOpNode::LHS() {
    if(mchildren.size() > 0) {
        return mchildren[0];
    }
    return nullptr;
}

AstNode* BinOpNode::RHS() {
    if(mchildren.size() > 1) {
        return mchildren[1];
    }
    return nullptr;
}

void BinOpNode::setLHS(AstNode* ast) {
        mchildren[0] = ast;
}

void BinOpNode::setRHS(AstNode* ast) {
        mchildren[1] = ast;
}

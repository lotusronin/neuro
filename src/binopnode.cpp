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

AstNodeType BinOpNode::nodeType() {
    return AstNodeType::BinOp;
}

void BinOpNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

void BinOpNode::setOp(std::string& op) {
    mop = op;
    //TODO(marcus): Not sure I like using constants here, maybe make them enums?
    if(op.compare("+") == 0 || op.compare("-") == 0) {
        mpriority = 1;
    } else if(op.compare("*") == 0 || op.compare("/") == 0) {
        mpriority = 2;
    } else {
        mpriority = 3;
    }
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

int BinOpNode::getPriority() {
    return mpriority;
}

void BinOpNode::setToken(Token& t) {
    mtoken = t;
    setOp(t.token);
}

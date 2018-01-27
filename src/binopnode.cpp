#include "binopnode.h"
#include <iostream>

int BinOpNode::count = 0;
int BinOpNode::deleted = 0;
int BinOpNode::constructed = 0;

BinOpNode::BinOpNode() {
    id = BinOpNode::count;
    BinOpNode::count++;
    BinOpNode::constructed++;
    unaryOp = false;
    mchildren.reserve(2);
    opOverload = nullptr;
    mnodet = AstNodeType::BinOp;
}

BinOpNode::~BinOpNode() {
    BinOpNode::deleted += 1;
}

void BinOpNode::setOp(const char* op) {
    mop = op;
}

const char* BinOpNode::getOp() const {
    return mop;
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

void BinOpNode::setToken(Token& t) {
    mtoken = t;
    setOp(t.token);
}

SemanticType BinOpNode::getType() const {
    return mtypeinfo.type;
}

void BinOpNode::printDeleted() {
    std::cout << "Constructed binop nodes " << BinOpNode::constructed << '\n';
    std::cout << "Deleted binop nodes: " << BinOpNode::deleted << '\n';
}

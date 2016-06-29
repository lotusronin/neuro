#include "constantnode.h"
#include <iostream>

int ConstantNode::count = 0;

ConstantNode::ConstantNode() {
    id = ConstantNode::count;
    ConstantNode::count++;
}

ConstantNode::~ConstantNode() {
}

void ConstantNode::makeGraph(std::ofstream& outfile) {
    outfile << "constant" << id << ";\n";
    outfile << "constant" << id << "[label=\"" << mval << "\ntype: " << mstype << "\"];\n";
    for (auto child : mchildren) {
        outfile << "constant" << id << " -> ";
        child->makeGraph(outfile);
    }
}

AstNodeType ConstantNode::nodeType() {
    return AstNodeType::Const;
}

void ConstantNode::addChild(AstNode* node) {
    std::cerr << "Should not be adding child to constant!\n";
}

std::vector<AstNode*>* ConstantNode::getChildren() {
    return &mchildren;
}

void ConstantNode::setVal(std::string& val) {
    mval = val;
}

void ConstantNode::setToken(const Token& t) {
    mtoken = t;
    if(t.type == TokenType::intlit) {
        mstype = SemanticType::Int;
    } else if(t.type == TokenType::floatlit) {
        mstype = SemanticType::Float;
    }
}
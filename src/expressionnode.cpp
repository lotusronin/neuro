#include "expressionnode.h"
#include <iostream>

int ExpressionNode::count = 0;

ExpressionNode::ExpressionNode() {
    id = ExpressionNode::count;
    ExpressionNode::count++;
}

ExpressionNode::~ExpressionNode() {
}

void ExpressionNode::makeGraph(std::ofstream& outfile) {
    //implement this
    outfile << "expression"<<id<<";\n";
    outfile << "expression"<<id<<"[label=\""<<mexprkind<<"\"];\n";
    for (auto child : mchildren) {
        outfile << "expression" << id << " -> ";
        child->makeGraph(outfile);
    }
}

AstNodeType ExpressionNode::nodeType()() {
    return AstNodeType::Expression;
}

void ExpressionNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

void ExpressionNode::setExprKind(std::string& kind) {
    mexprkind = kind;
}

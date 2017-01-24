#include "castnode.h"
#include <iostream>

int CastNode::count = 0;

CastNode::CastNode() {
    id = CastNode::count;
    CastNode::count++;
}

CastNode::~CastNode() {
}

void CastNode::makeGraph(std::ofstream& outfile) {
    //implement this
    outfile << "cast"<<id<<";\n";
    outfile << "cast"<<id<<"[label=\"cast to " << toType << "\"];\n";
    for (auto child : mchildren) {
        outfile << "cast" << id << " -> ";
        child->makeGraph(outfile);
    }
}

AstNodeType CastNode::nodeType() {
    return AstNodeType::Cast;
}

void CastNode::addChild(AstNode* n) {
    //TODO(marcus): should we allow more than 1 child for Cast nodes?
    mchildren.push_back(n);
}

std::vector<AstNode*>* CastNode::getChildren() {
    return &mchildren;
}

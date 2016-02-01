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
    outfile << "constant" << id << "[label=\"" << mval << "\"];\n";
    for (auto child : mchildren) {
        outfile << "constant" << id << " -> ";
        child->makeGraph(outfile);
    }
}

AstNodeType ConstantNode::type() {
    return AstNodeType::Const;
}

void ConstantNode::addChild(AstNode* node) {
    std::cerr << "Should not be adding child to constant!\n";
}

void ConstantNode::setVal(std::string& val) {
    mval = val;
}

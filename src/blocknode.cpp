#include "blocknode.h"
#include <iostream>

int BlockNode::count = 0;

BlockNode::BlockNode() {
    id = BlockNode::count;
    BlockNode::count++;
}

BlockNode::~BlockNode() {
}

void BlockNode::makeGraph(std::ofstream& outfile) {
    outfile << "block" << id << ";\n";
    outfile << "block" << id << "[label=\"block\"];\n";
    for (auto s : mstatements) {
        outfile << "block" << id << " -> ";
        s->makeGraph(outfile);
    }
}

AstNodeType BlockNode::type() {
    return AstNodeType::Block;
}

void BlockNode::addChild(AstNode* node) {
    mstatements.push_back(node);
}

AstNode* BlockNode::lastChild() {
    return mstatements.back();
}

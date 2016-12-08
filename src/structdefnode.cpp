#include "structdefnode.h"
#include <iostream>

int StructDefNode::count = 0;

StructDefNode::StructDefNode() {
    id = StructDefNode::count;
    std::cout << "StructDefNode made! ID: " << id << "\n";
    StructDefNode::count++;
}

StructDefNode::~StructDefNode() {
}

void StructDefNode::makeGraph(std::ofstream& outfile) {
    //implement this
    std::cout << "struct"<<id<<" outputting to dot file...\n";
    std::cout << "Child size is " << mchildren.size() << "\n";
    outfile << "struct"<<id<<";\n";
    outfile << "struct"<<id<<"[label=\"struct " <<  ident << "\"];\n";
    for (auto child : mchildren) {
        outfile << "struct" << id << " -> ";
        child->makeGraph(outfile);
    }
}

AstNodeType StructDefNode::nodeType() {
    return AstNodeType::StructDef;
}

void StructDefNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

std::vector<AstNode*>* StructDefNode::getChildren() {
    return &mchildren;
}

std::string& StructDefNode::getIdent() {
    return ident;
}

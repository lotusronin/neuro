#include "typenode.h"
#include <iostream>

int TypeNode::count = 0;

TypeNode::TypeNode() {
    id = TypeNode::count;
    TypeNode::count++;
}

TypeNode::~TypeNode() {
}

void TypeNode::makeGraph(std::ofstream& outfile) {
    //implement this
    outfile << "type"<<id<<";\n";
    outfile << "type"<<id<<"[label=\""<<mname<<"\"];\n";
}

AstNodeType TypeNode::type() {
    return AstNodeType::Type;
}

void TypeNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

void TypeNode::setTypeName(std::string tname) {
    mname = tname;
}

std::vector<AstNode*>* TypeNode::getChildren() {
    return &mchildren;
}

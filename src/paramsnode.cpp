#include "paramsnode.h"
#include <iostream>

int ParamsNode::count = 0;

ParamsNode::ParamsNode() {
    id = ParamsNode::count;
    ParamsNode::count++;
}

ParamsNode::~ParamsNode() {
}

void ParamsNode::makeGraph(std::ofstream& outfile) {
    //implement this
    outfile << "param"<<id<<";\n";
    outfile << "param"<<id<<"[label=\""<<mname<<"\"];\n";
    for (auto child : mchildren) {
        outfile << "param" << id << " -> ";
        child->makeGraph(outfile);
    }
}

AstNodeType ParamsNode::type() {
    return AstNodeType::Params;
}

void ParamsNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

void ParamsNode::addParamName(std::string name) {
    mname = name;
}

std::vector<AstNode*>* ParamsNode::getChildren() {
    return &mchildren;
}

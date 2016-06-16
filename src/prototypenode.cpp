#include "prototypenode.h"
#include <iostream>

int PrototypeNode::count = 0;

PrototypeNode::PrototypeNode() {
    id = PrototypeNode::count;
    PrototypeNode::count++;
}

PrototypeNode::~PrototypeNode() {
}

void PrototypeNode::makeGraph(std::ofstream& outfile) {
    //implement this
    outfile << "prototype" << id << ";\n";
    outfile << "prototype" << id << "[label=\"extern fn "<<mfuncname<<"\"];\n";
    for (auto param : mparams) {
        outfile << "prototype" << id << " -> ";
        param->makeGraph(outfile);
    }
}

AstNodeType PrototypeNode::nodeType() {
    return AstNodeType::Prototype;
}

void PrototypeNode::addParams(AstNode* node) {
    mparams.push_back(node);
}

void PrototypeNode::addChild(AstNode* node) {
    mparams.push_back(node);
}

void PrototypeNode::addFuncName(std::string funcname) {
    mfuncname = funcname;
}

std::vector<AstNode*>* PrototypeNode::getChildren() {
    return &mparams;
}

std::vector<AstNode*>* PrototypeNode::getParameters() {
    std::vector<AstNode*>* params = new std::vector<AstNode*>(mparams.begin(),mparams.end()-1);
    return params;
}

SemanticType PrototypeNode::getType() {
    return mparams[mparams.size()-1]->getType();
}

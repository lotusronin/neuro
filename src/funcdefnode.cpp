#include "funcdefnode.h"
#include <iostream>

int FuncDefNode::count = 0;

FuncDefNode::FuncDefNode() {
    id = FuncDefNode::count;
    FuncDefNode::count++;
}

FuncDefNode::~FuncDefNode() {
}

void FuncDefNode::makeGraph(std::ofstream& outfile) {
    //implement this
    outfile << "funcDef" << id << ";\n";
    outfile << "funcDef" << id << "[label=\"fn "<<mfuncname<<"\"];\n";
    for (auto param : mparams) {
        outfile << "funcDef" << id << " -> ";
        param->makeGraph(outfile);
    }
}

AstNodeType FuncDefNode::nodeType() {
    return AstNodeType::FuncDef;
}

void FuncDefNode::addParams(AstNode* node) {
    mparams.push_back(node);
}

void FuncDefNode::addChild(AstNode* node) {
    mparams.push_back(node);
}

void FuncDefNode::addFuncName(std::string funcname) {
    mfuncname = funcname;
}

std::vector<AstNode*>* FuncDefNode::getChildren() {
    return &mparams;
}

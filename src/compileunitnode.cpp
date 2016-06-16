#include "compileunitnode.h"
#include <iostream>

int CompileUnitNode::count = 0;

CompileUnitNode::CompileUnitNode() {
    id = CompileUnitNode::count;
    CompileUnitNode::count++;
}

CompileUnitNode::~CompileUnitNode() {
}

void CompileUnitNode::makeGraph(std::ofstream& outfile) {
    //implement this
    outfile << "CompilationUnit"<<id<<";\n";
    outfile << "CompilationUnit"<<id<<"[label=\"" << mname << "\"];\n";
    for (auto child : mchildren) {
        outfile << "CompilationUnit"<<id<<" -> ";
        child->makeGraph(outfile);
    }
}

AstNodeType CompileUnitNode::nodeType() {
    return AstNodeType::CompileUnit;
}

void CompileUnitNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

void CompileUnitNode::setFileName(std::string name) {
    mname = name;
}

std::string CompileUnitNode::getFileName() {
    return mname;
}

std::vector<AstNode*>* CompileUnitNode::getChildren() {
    return &mchildren;
}

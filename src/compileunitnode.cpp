#include "compileunitnode.h"

int CompileUnitNode::count = 0;

CompileUnitNode::CompileUnitNode() {
    id = CompileUnitNode::count;
    CompileUnitNode::count++;
    mnodet = AstNodeType::CompileUnit;
}

CompileUnitNode::~CompileUnitNode() {
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

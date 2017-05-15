#include "prototypenode.h"
#include <iostream>

int PrototypeNode::count = 0;

PrototypeNode::PrototypeNode() {
    id = PrototypeNode::count;
    PrototypeNode::count++;
}

PrototypeNode::~PrototypeNode() {
}

AstNodeType PrototypeNode::nodeType() {
    return AstNodeType::Prototype;
}

void PrototypeNode::addParams(AstNode* node) {
    mchildren.push_back(node);
}

void PrototypeNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

void PrototypeNode::addFuncName(std::string funcname) {
    mfuncname = funcname;
}

std::vector<AstNode*>* PrototypeNode::getChildren() {
    return &mchildren;
}

std::vector<AstNode*>* PrototypeNode::getParameters() {
    std::vector<AstNode*>* params = new std::vector<AstNode*>(mchildren.begin(),mchildren.end());
    return params;
}

SemanticType PrototypeNode::getType() {
    return mtypeinfo.type;
}

#include "funcdefnode.h"
#include "astnodetypes.h"
#include <iostream>

int FuncDefNode::count = 0;

FuncDefNode::FuncDefNode() {
    id = FuncDefNode::count;
    FuncDefNode::count++;
}

FuncDefNode::~FuncDefNode() {
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

SemanticType FuncDefNode::getType() {
    //TODO(marcus): should not hard-code node locations
    //AstNode* tn = mparams[mparams.size()-2];
    //mstype = tn->getType();
    //mstype = mytpeinfo.type;
    return mtypeinfo.type;
    //return mstype;
}

std::vector<AstNode*>* FuncDefNode::getParameters() {
    std::vector<AstNode*>* params = new std::vector<AstNode*>(mparams.begin(),mparams.end()-1);
    return params;
}

AstNode* FuncDefNode::getFunctionBody() {
    return mparams.back();
}

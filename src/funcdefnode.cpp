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
    //mparams.push_back(node);
    mchildren.push_back(node);
}

void FuncDefNode::addChild(AstNode* node) {
    //mparams.push_back(node);
    mchildren.push_back(node);
}

void FuncDefNode::addFuncName(std::string funcname) {
    mfuncname = funcname;
}

std::vector<AstNode*>* FuncDefNode::getChildren() {
    //return &mparams;
    return &mchildren;
}

SemanticType FuncDefNode::getType() {
    return mtypeinfo.type;
}

std::vector<AstNode*>* FuncDefNode::getParameters() {
    //std::vector<AstNode*>* params = new std::vector<AstNode*>(mparams.begin(),mparams.end()-1);
    std::vector<AstNode*>* params = new std::vector<AstNode*>(mchildren.begin(),mchildren.end()-1);
    return params;
}

AstNode* FuncDefNode::getFunctionBody() {
    //return mparams.back();
    return mchildren.back();
}

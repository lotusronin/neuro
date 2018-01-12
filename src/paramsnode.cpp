#include "paramsnode.h"

int ParamsNode::count = 0;

ParamsNode::ParamsNode() {
    id = ParamsNode::count;
    ParamsNode::count++;
    mnodet = AstNodeType::Params;
}

ParamsNode::~ParamsNode() {
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

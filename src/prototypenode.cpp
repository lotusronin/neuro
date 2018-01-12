#include "prototypenode.h"

int PrototypeNode::count = 0;

PrototypeNode::PrototypeNode() {
    id = PrototypeNode::count;
    PrototypeNode::count++;
    mnodet = AstNodeType::Prototype;
}

PrototypeNode::~PrototypeNode() {
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

ArrayView PrototypeNode::getParameters() {
    auto s = mchildren.size();
    auto p = s ? &mchildren[0] : nullptr;
    ArrayView params(p,s);
    return params;
}

SemanticType PrototypeNode::getType() {
    return mtypeinfo.type;
}

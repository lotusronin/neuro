#include "constantnode.h"
#include <iostream>

int ConstantNode::count = 0;

ConstantNode::ConstantNode() {
    id = ConstantNode::count;
    ConstantNode::count++;
    mnodet = AstNodeType::Const;
}

ConstantNode::~ConstantNode() {
}

void ConstantNode::addChild(AstNode* node) {
    std::cerr << "Should not be adding child to constant!\n";
}

std::vector<AstNode*>* ConstantNode::getChildren() {
    return &mchildren;
}

void ConstantNode::setVal(std::string& val) {
    mval = val;
}

std::string ConstantNode::getVal() {
    return mval;
}

void ConstantNode::setToken(const Token& t) {
    mtoken = t;
    SemanticType mstype = SemanticType::Typeless;
    if(t.type == TokenType::intlit) {
        mstype = SemanticType::intlit;
    } else if(t.type == TokenType::floatlit) {
        mstype = SemanticType::floatlit;
    } else if(t.type == TokenType::strlit) {
        //TODO(marcus): maybe fix this up?
        mstype = SemanticType::Char;
        mtypeinfo.indirection = 1;
    } else if(t.type == TokenType::charlit) {
        mstype = SemanticType::Char;
    }
    mtypeinfo.type = mstype;
}

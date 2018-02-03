#include "constantnode.h"
#include <iostream>
#include <cstring>

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

void ConstantNode::setVal(const char* val) {
    mval = val;
    if(mtypeinfo.type == SemanticType::nulllit) {
        mval = "0";
    }
}

const char* ConstantNode::getVal() const {
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
        //TODO(marcus): this might break if someone else tries to set mtypeinfo later
        mstype = SemanticType::Char;
        mtypeinfo.pindirection = 1;
    } else if(t.type == TokenType::charlit) {
        mstype = SemanticType::Char;
    } else if(t.type == TokenType::vnull) {
        mstype = SemanticType::nulllit;
    }
    mtypeinfo.type = mstype;
}

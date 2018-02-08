#include "loopnode.h"
#include <cassert>

int LoopNode::count = 0;

LoopNode::LoopNode(AstNodeType ntype) {
    id = LoopNode::count;
    LoopNode::count++;
    mnodet = ntype;
}

LoopNode::LoopNode(LoopNode* n) {
    id = LoopNode::count;
    LoopNode::count++;
    mnodet = n->mnodet;
    mtoken = n->mtoken;
}

LoopNode::~LoopNode() {
}

AstNode* LoopNode::getInit() {
    assert(mnodet == AstNodeType::ForLoop);
    return mchildren.at(0);
}

AstNode* LoopNode::getConditional() {
    //for loop at 1
    //while loop at 0
    int idx = 0;
    if(mnodet == AstNodeType::ForLoop) idx = 1;
    return mchildren.at(idx);
}

AstNode* LoopNode::getUpdate() {
    assert(mnodet == AstNodeType::ForLoop);
    return mchildren.at(2);
}

AstNode* LoopNode::getBody() {
    //for loop at 3
    //while loop at 1
    int idx = 1;
    if(mnodet == AstNodeType::ForLoop) idx = 3;
    return mchildren.at(idx);
}

int LoopNode::getId() const {
    return id;
}

#include "castnode.h"

int CastNode::count = 0;

CastNode::CastNode() {
    id = CastNode::count;
    CastNode::count++;
    mnodet = AstNodeType::Cast;
}

CastNode::CastNode(CastNode* n) {
    id = CastNode::count;
    CastNode::count++;
    mnodet = AstNodeType::Cast;
    mtoken = n->mtoken;
    mtypeinfo = n->mtypeinfo;
    fromType = n->fromType;
}

CastNode::~CastNode() {
}

void CastNode::addChild(AstNode* n) {
    //TODO(marcus): should we allow more than 1 child for Cast nodes?
    mchildren.push_back(n);
}

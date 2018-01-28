#include "structdefnode.h"

int StructDefNode::count = 0;

StructDefNode::StructDefNode(AstNodeType nodet) {
    id = StructDefNode::count;
    StructDefNode::count++;
    foreign = false;
    mnodet = nodet;
}

StructDefNode::~StructDefNode() {
}

std::string& StructDefNode::getIdent() {
    return ident;
}

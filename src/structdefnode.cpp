#include "structdefnode.h"

int StructDefNode::count = 0;

StructDefNode::StructDefNode() {
    id = StructDefNode::count;
    StructDefNode::count++;
    foreign = false;
    mnodet = AstNodeType::StructDef;
}

StructDefNode::~StructDefNode() {
}

std::string& StructDefNode::getIdent() {
    return ident;
}

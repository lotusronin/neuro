#include "sizeofnode.h"

int SizeOfNode::count = 0;

SizeOfNode::SizeOfNode() {
    id = SizeOfNode::count;
    SizeOfNode::count++;
    mnodet = AstNodeType::SizeOf;
}

SizeOfNode::~SizeOfNode() {
}

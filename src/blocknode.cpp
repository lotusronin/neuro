#include "blocknode.h"

int BlockNode::count = 0;

BlockNode::BlockNode() {
    id = BlockNode::count;
    BlockNode::count++;
    mnodet = AstNodeType::Block;
}

BlockNode::~BlockNode() {
}

int BlockNode::getId() {
    return id;
}

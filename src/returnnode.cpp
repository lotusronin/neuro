#include "returnnode.h"

int ReturnNode::count = 0;

ReturnNode::ReturnNode() {
    id = ReturnNode::count;
    ReturnNode::count++;
    mchildren.reserve(1);
    mnodet = AstNodeType::RetStmnt;
}

ReturnNode::~ReturnNode() {
}

#include "funccallnode.h"

int FuncCallNode::count = 0;

FuncCallNode::FuncCallNode() {
    id = FuncCallNode::count;
    FuncCallNode::count++;
    mnodet = AstNodeType::FuncCall;
    func = nullptr;
}

FuncCallNode::~FuncCallNode() {
}

void FuncCallNode::addFuncName(std::string funcname) {
    mfuncname = funcname;
}

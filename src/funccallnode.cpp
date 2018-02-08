#include "funccallnode.h"

int FuncCallNode::count = 0;

FuncCallNode::FuncCallNode() {
    id = FuncCallNode::count;
    FuncCallNode::count++;
    mnodet = AstNodeType::FuncCall;
    func = nullptr;
}

FuncCallNode::FuncCallNode(FuncCallNode* n) {
    id = FuncCallNode::count;
    FuncCallNode::count++;
    mnodet = AstNodeType::FuncCall;
    func = n->func;
    mfuncname = n->mfuncname;
    scopes = n->scopes;
}

FuncCallNode::~FuncCallNode() {
}

void FuncCallNode::addFuncName(std::string funcname) {
    mfuncname = funcname;
}

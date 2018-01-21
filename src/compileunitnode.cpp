#include "compileunitnode.h"

int CompileUnitNode::count = 0;

CompileUnitNode::CompileUnitNode() {
    id = CompileUnitNode::count;
    CompileUnitNode::count++;
    mnodet = AstNodeType::CompileUnit;
}

CompileUnitNode::~CompileUnitNode() {
}

void CompileUnitNode::setFileName(std::string name) {
    mname = name;
}

std::string CompileUnitNode::getFileName() const {
    return mname;
}

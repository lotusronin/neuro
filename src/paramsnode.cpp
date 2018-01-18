#include "paramsnode.h"

int ParamsNode::count = 0;

ParamsNode::ParamsNode() {
    id = ParamsNode::count;
    ParamsNode::count++;
    mnodet = AstNodeType::Params;
}

ParamsNode::~ParamsNode() {
}

void ParamsNode::addParamName(std::string name) {
    mname = name;
}

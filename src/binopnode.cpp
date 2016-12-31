#include "binopnode.h"
#include <iostream>

int BinOpNode::count = 0;

BinOpNode::BinOpNode() {
    id = BinOpNode::count;
    BinOpNode::count++;
}

BinOpNode::~BinOpNode() {
}

void BinOpNode::makeGraph(std::ofstream& outfile) {
    outfile << "binop" << id << ";\n";
    outfile << "binop" << id << "[label=\"" << mop << "\ntype: " << mstype << "\"];\n";
    for (auto child : mchildren) {
        outfile << "binop" << id << " -> ";
        child->makeGraph(outfile);
    }
}

AstNodeType BinOpNode::nodeType() {
    return AstNodeType::BinOp;
}

void BinOpNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

void BinOpNode::setOp(std::string& op) {
    mop = op;
    //TODO(marcus): Not sure I like using constants here, maybe make them enums?
    //FIXME(marcus): I really don't like using constants here. Definitely make them enums.
    if(op.compare("||") == 0) {
        mpriority = -7;
    } else if(op.compare("&&") == 0) {
        mpriority = -6;
    } else if(op.compare("|") == 0) {
        mpriority = -5;
    } else if(op.compare("^") == 0) {
        mpriority = -4;
    } else if(op.compare("==") == 0 || op.compare("!=") == 0) {
        mpriority = -2;
    } else if(op.compare("<") == 0 || op.compare(">") == 0 || op.compare("<=") == 0 || op.compare(">=") == 0) {
        mpriority = -1;
    } else if(op.compare("+") == 0 || op.compare("-") == 0) {
        mpriority = 1;
    } else if(op.compare("*") == 0 || op.compare("/") == 0 || op.compare("%")) {
        mpriority = 2;
    } else if(op.compare(".") == 0) {
        mpriority = 4;
    } else if(op.compare("@") == 0 || op.compare("&") == 0 || op.compare("!") || op.compare("~")) {
        //TODO(marcus): bitwise & has a lower priority than address-of!!!
        mpriority = 5;
    } else {
        mpriority = 3;
    }
}

std::string BinOpNode::getOp() {
    return mop;
}

std::vector<AstNode*>* BinOpNode::getChildren() {
    return &mchildren;
}

AstNode* BinOpNode::LHS() {
    if(mchildren.size() > 0) {
        return mchildren[0];
    }
    return nullptr;
}

AstNode* BinOpNode::RHS() {
    if(mchildren.size() > 1) {
        return mchildren[1];
    }
    return nullptr;
}

void BinOpNode::setLHS(AstNode* ast) {
        mchildren[0] = ast;
}

void BinOpNode::setRHS(AstNode* ast) {
        mchildren[1] = ast;
}

int BinOpNode::getPriority() {
    return mpriority;
}

void BinOpNode::setToken(Token& t) {
    mtoken = t;
    setOp(t.token);
}

SemanticType BinOpNode::getType() {
    //FIXME(marcus): This is utterly broken.
    //The . and -> operators don't care about lhs type
    //also this breaks for math operators since it always
    //promotes to the rhs type, instead of largest
    if(mpriority == 3) {
        mstype = mchildren[0]->getType();
    } else {
        auto lhs_type = LHS()->getType();
        auto rhs_type = RHS()->getType();

        if(lhs_type == rhs_type) {
            mstype = lhs_type;
        } else {
            mstype = rhs_type;
        }
    }

    return mstype;
}

#include "typenode.h"
#include <iostream>

int TypeNode::count = 0;

TypeNode::TypeNode() {
    id = TypeNode::count;
    TypeNode::count++;
}

TypeNode::~TypeNode() {
}

void TypeNode::makeGraph(std::ofstream& outfile) {
    //implement this
    outfile << "type"<<id<<";\n";
    outfile << "type"<<id<<"[label=\"";
    for(int i = 0; i < mindirection; i++) {
        outfile << "*";
    }
    outfile <<mname<<"\"];\n";
}

AstNodeType TypeNode::nodeType() {
    return AstNodeType::Type;
}

void TypeNode::addChild(AstNode* node) {
    mchildren.push_back(node);
}

void TypeNode::setTypeName(std::string tname) {
    mname = tname;
}

bool TypeNode::isPointerType() {
    return (mindirection > 0);
}

std::vector<AstNode*>* TypeNode::getChildren() {
    return &mchildren;
}

void TypeNode::setToken(const Token& t) {
    mtoken = t;
    mname = t.token;
    switch(t.type) {
        case TokenType::tuchar:
        case TokenType::tchar:
            mstype = SemanticType::Char;
            break;
        case TokenType::tint:
            mstype =SemanticType::Int;
            break;
        case TokenType::tbool:
            mstype = SemanticType::Bool;
            break;
        case TokenType::tfloat:
            mstype = SemanticType::Float;
            break;
        case TokenType::tdouble:
            mstype = SemanticType::Double;
            break;
        case TokenType::tvoid:
            mstype = SemanticType::Void;
            break;
        case TokenType::id:
            mstype = SemanticType::User;
            std::cout << "User Defined Type!!! WOOOOOOOOOO\n";
            break;
        default:
            mstype = SemanticType::Typeless;
            break;
    }
}

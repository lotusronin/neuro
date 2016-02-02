#include "asttransforms.h"
#include "astnodetypes.h"
#include "astnode.h"
#include <iostream>

#define ANT AstNodeType

void collapseExpressionChains(AstNode* ast) {
    std::vector<AstNode*>* vec = ast->getChildren();
    for(unsigned int i = 0; i < vec->size(); i++) {
        AstNode* child = (*vec)[i];
        if(child->type() == ANT::BinOp) {
            while(child->type() == ANT::BinOp) {
                BinOpNode* node = (BinOpNode*)child;
                std::string op = node->getOp();
                if(op.compare("expression") == 0) {
                    if(node->mchildren.size() == 1) {
                        std::cout << "Deleting child!!!\n";
                        (*vec)[i] = node->mchildren[0];
                        delete node;
                        child = (*vec)[i];
                    }
                } else if(op.compare("multdivexpr") == 0) {
                    if(node->mchildren.size() == 1) {
                        std::cout << "Deleting child!!!\n";
                        (*vec)[i] = node->mchildren[0];
                        delete node;
                        child = (*vec)[i];
                    }
                } else if(op.compare("parenexpr") == 0) {
                    std::cout << "Deleting child!!!\n";
                    (*vec)[i] = node->mchildren[0];
                    delete node;
                    child = (*vec)[i];
                } else {
                    break;
                }
            }
        }
    }
    for(auto c : (*vec)) {
        collapseExpressionChains(c);
    }
}

void checkContinueBreak(AstNode* ast, int loopDepth) {
    AstNodeType type = ast->type();
    if(type == AstNodeType::LoopStmt) {
        if(loopDepth == 0) {
            std::cout << "Error, Break or Continue used outside of a loop!\n";
        }
    }

    int nextLoopDepth = loopDepth;
    if(type == AstNodeType::ForLoop || type == AstNodeType::WhileLoop) {
        nextLoopDepth += 1;
    }
    for(auto c : (*(ast->getChildren()))) {
        checkContinueBreak(c, nextLoopDepth);
    }
}

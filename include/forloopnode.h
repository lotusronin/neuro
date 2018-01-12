#ifndef FORLOOPNODE_H
#define FORLOOPNODE_H

#include <vector>
#include <string>
#include "astnode.h"

class ForLoopNode : public AstNode {
    public:
        static int count;
        ForLoopNode();
        ~ForLoopNode();
        void addChild(AstNode* node);
        std::vector<AstNode*>* getChildren();
        AstNode* getConditional();
        AstNode* getUpdate();
        AstNode* getInit();
        AstNode* getBody();
        int getId();
        int id;
};



#endif

#ifndef LOOPNODE_H
#define LOOPNODE_H

#include <vector>
#include "astnode.h"

class LoopNode : public AstNode {
    public:
        static int count;
        LoopNode(AstNodeType ntype);
        ~LoopNode();
        void addChild(AstNode* node);
        AstNode* getConditional();
        AstNode* getUpdate();
        AstNode* getInit();
        AstNode* getBody();
        int getId();
        int id;
};

#endif

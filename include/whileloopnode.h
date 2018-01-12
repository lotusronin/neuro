#ifndef WHILELOOPNODE_H
#define WHILELOOPNODE_H

#include <vector>
#include <string>
#include "astnode.h"

class WhileLoopNode : public AstNode {
    public:
        static int count;
        WhileLoopNode();
        ~WhileLoopNode();
        void addChild(AstNode* node);
        std::vector<AstNode*>* getChildren();
        AstNode* getConditional();
        AstNode* getBody();
        int getId();
        int id;
};



#endif

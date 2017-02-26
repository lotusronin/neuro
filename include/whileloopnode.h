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
        AstNodeType nodeType();
        void addChild(AstNode* node);
        std::vector<AstNode*>* getChildren();
        //std::vector<AstNode*> mstatements;
        AstNode* getConditional();
        AstNode* getBody();
        int getId();
    //private:
        int id;
};



#endif

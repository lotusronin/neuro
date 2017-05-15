#ifndef IFNODE_H
#define IFNODE_H

#include <vector>
#include <string>
#include "astnode.h"

class IfNode : public AstNode {
    public:
        static int count;
        IfNode();
        ~IfNode();
        AstNodeType nodeType();
        void addChild(AstNode* node);
        std::vector<AstNode*>* getChildren();
        AstNode* getConditional();
        AstNode* getThen();
        AstNode* getElse();
    //private:
        int id;
};



#endif

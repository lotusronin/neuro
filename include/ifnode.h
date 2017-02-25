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
        std::vector<AstNode*> mstatements;
        AstNode* getConditional();
        AstNode* getThen();
    //private:
        int id;
};



#endif

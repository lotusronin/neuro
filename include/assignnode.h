#ifndef ASSIGNNODE_H
#define ASSIGNNODE_H

#include <vector>
#include <string>
#include "astnode.h"

class AssignNode : public AstNode {
    public:
        static int count;
        AssignNode();
        ~AssignNode();
        AstNodeType nodeType();
        void addChild(AstNode* node);
        std::vector<AstNode*>* getChildren();
        AstNode* getLHS();
        AstNode* getRHS();
        std::vector<AstNode*> mchildren;
        int id;
};



#endif

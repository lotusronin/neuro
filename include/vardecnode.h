#ifndef VARDECNODE_H
#define VARDECNODE_H

#include <vector>
#include <string>
#include "astnode.h"

class VarDecNode : public AstNode {
    public:
        static int count;
        VarDecNode();
        ~VarDecNode();
        void addChild(AstNode* node);
        std::vector<AstNode*>* getChildren();
        AstNode* getLHS();
        int id;
};



#endif

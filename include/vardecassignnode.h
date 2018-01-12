#ifndef VARDECASSIGNNODE_H
#define VARDECASSIGNNODE_H

#include <vector>
#include <string>
#include "astnode.h"

class VarDecAssignNode : public AstNode {
    public:
        static int count;
        VarDecAssignNode();
        ~VarDecAssignNode();
        void addChild(AstNode* node);
        std::vector<AstNode*>* getChildren();
        AstNode* getLHS();
        AstNode* getRHS();
        int id;
};



#endif

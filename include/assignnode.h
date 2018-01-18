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
        void addChild(AstNode* node);
        AstNode* getLHS();
        AstNode* getRHS();
        int id;
};



#endif

#ifndef SIZEOFNODE_H
#define SIZEOFNODE_H

#include <vector>
#include <string>
#include "astnode.h"

class SizeOfNode : public AstNode {
    public:
        static int count;
        SizeOfNode();
        ~SizeOfNode();
        AstNodeType nodeType();
        void addChild(AstNode* node);
        std::vector<AstNode*>* getChildren();
        int id;
};

#endif
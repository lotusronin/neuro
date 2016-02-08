#ifndef ARGSNODE_H
#define ARGSNODE_H

#include <vector>
#include "astnode.h"

class ArgsNode : public AstNode {
    public:
        ArgsNode();
        ~ArgsNode();
        void makeGraph();
        AstNodeType nodeType();
        void addChild(AstNode* node);
        std::vector<AstNode*>* getChildren();
        std::vector<AstNode*> mchildren;
};

#endif

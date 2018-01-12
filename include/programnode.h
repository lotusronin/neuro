#ifndef PROGRAMNODE_H
#define PROGRAMNODE_H

#include <vector>
#include "astnode.h"

class ProgramNode : public AstNode {
    public:
        ProgramNode();
        ~ProgramNode();
        void addChild(AstNode* node);
        std::vector<AstNode*>* getChildren();
};


#endif

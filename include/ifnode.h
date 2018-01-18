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
        AstNode* getConditional();
        AstNode* getThen();
        AstNode* getElse();
        int id;
};



#endif

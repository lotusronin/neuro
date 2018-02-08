#ifndef RETURNNODE_H
#define RETURNNODE_H

#include <vector>
#include "astnode.h"

class ReturnNode : public AstNode {
    public:
        static int count;
        ReturnNode();
        ReturnNode(ReturnNode* n);
        ~ReturnNode();
        int id;
};

#endif

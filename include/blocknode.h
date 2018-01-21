#ifndef BLOCKNODE_H
#define BLOCKNODE_H

#include <vector>
#include "astnode.h"

class BlockNode : public AstNode {
    public:
        static int count;
        BlockNode();
        ~BlockNode();
        int getId() const;
        int id;
};



#endif

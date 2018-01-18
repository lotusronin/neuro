#ifndef VARDECLNODE_H
#define VARDECLNODE_H

#include <vector>
#include "astnode.h"

class VarDeclNode : public AstNode {
    public:
        static int count;
        VarDeclNode(AstNodeType ntype);
        ~VarDeclNode();
        AstNode* getLHS();
        AstNode* getRHS();
        int id;
};


#endif

#ifndef CASTNODE_H
#define CASTNODE_H

#include <iostream>
#include <vector>
#include "astnode.h"

class CastNode : public AstNode {
    public:
        static int count;
        CastNode();
        ~CastNode();
        void addChild(AstNode* node);
        TypeInfo fromType;
        TypeInfo toType;
        int id;
};

#endif //CASTNODE_H

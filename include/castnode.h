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
        void makeGraph(std::ofstream& outfile);
        AstNodeType nodeType();
        void addChild(AstNode* node);
        std::vector<AstNode*>* getChildren();
        std::vector<AstNode*> mchildren;
        TypeInfo fromType;
        TypeInfo toType;
    private:
        int id;
};

#endif //CASTNODE_H

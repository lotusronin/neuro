#ifndef COMPILEUNITNODE_H
#define COMPILEUNITNODE_H

#include <vector>
#include "astnode.h"

class CompileUnitNode : public AstNode {
    public:
        static int count;
        CompileUnitNode();
        ~CompileUnitNode();
        void makeGraph(std::ofstream& outfile);
        AstNodeType type();
        void addChild(AstNode* node);
        std::vector<AstNode*> mchildren;
    private:
        int id;
};

#endif

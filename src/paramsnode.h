#ifndef PARAMSNODE_H
#define PARAMSNODE_H

#include <vector>
#include "astnode.h"

class ParamsNode : public AstNode {
    public:
        static int count;
        ParamsNode();
        ~ParamsNode();
        void makeGraph(std::ofstream& outfile);
        AstNodeType type();
        void addChild(AstNode* node);
        std::vector<AstNode*> mchildren;
    private:
        int id;
};

#endif

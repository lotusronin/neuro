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
        void makeGraph(std::ofstream& outfile);
        AstNodeType type();
        void addChild(AstNode* node);
        std::vector<AstNode*> mstatements;
    private:
        int id;
};



#endif

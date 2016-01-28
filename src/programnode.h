#ifndef PROGRAMNODE_H
#define PROGRAMNODE_H

#include <vector>
#include "astnode.h"

class ProgramNode : public AstNode {
    public:
        ProgramNode();
        ~ProgramNode();
        void makeGraph(std::ofstream& outfile);
        AstNodeType type();
        void addChild(AstNode* node);
        std::vector<AstNode*> mchildren;
};


#endif

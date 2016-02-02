#ifndef BLOCKNODE_H
#define BLOCKNODE_H

#include <vector>
#include <string>
#include "astnode.h"

class BlockNode : public AstNode {
    public:
        static int count;
        BlockNode();
        ~BlockNode();
        void makeGraph(std::ofstream& outfile);
        AstNodeType type();
        void addChild(AstNode* node);
        AstNode* lastChild();
        std::vector<AstNode*>* getChildren();
        std::vector<AstNode*> mstatements;
    private:
        int id;
};



#endif

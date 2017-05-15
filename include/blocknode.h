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
        AstNodeType nodeType();
        void addChild(AstNode* node);
        AstNode* lastChild();
        std::vector<AstNode*>* getChildren();
        int getId();
        //std::vector<AstNode*> mstatements;
    //private:
        int id;
};



#endif

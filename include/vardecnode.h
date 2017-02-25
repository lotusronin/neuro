#ifndef VARDECNODE_H
#define VARDECNODE_H

#include <vector>
#include <string>
#include "astnode.h"

class VarDecNode : public AstNode {
    public:
        static int count;
        VarDecNode();
        ~VarDecNode();
        AstNodeType nodeType();
        void addChild(AstNode* node);
        std::vector<AstNode*>* getChildren();
        std::vector<AstNode*> mchildren;
        AstNode* getLHS();
        AstNode* getRHS();
    //private:
        int id;
};



#endif

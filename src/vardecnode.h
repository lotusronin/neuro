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
        void makeGraph(std::ofstream& outfile);
        AstNodeType nodeType();
        void addChild(AstNode* node);
        std::vector<AstNode*>* getChildren();
        std::vector<AstNode*> mchildren;
    private:
        int id;
};



#endif

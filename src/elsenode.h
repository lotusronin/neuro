#ifndef ELSENODE_H
#define ELSENODE_H

#include <vector>
#include <string>
#include "astnode.h"

class ElseNode : public AstNode {
    public:
        static int count;
        ElseNode();
        ~ElseNode();
        void makeGraph(std::ofstream& outfile);
        AstNodeType nodeType();
        void addChild(AstNode* node);
        std::vector<AstNode*>* getChildren();
        std::vector<AstNode*> mstatements;
    private:
        int id;
};



#endif

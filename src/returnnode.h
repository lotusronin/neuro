#ifndef RETURNNODE_H
#define RETURNNODE_H

#include <vector>
#include "astnode.h"

class ReturnNode : public AstNode {
    public:
        static int count;
        ReturnNode();
        ~ReturnNode();
        void makeGraph(std::ofstream& outfile);
        AstNodeType type();
        void addChild(AstNode* node);
        std::vector<AstNode*>* getChildren();
        std::vector<AstNode*> mchildren;
    private:
        int id;
};

#endif

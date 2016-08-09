#ifndef FORLOOPNODE_H
#define FORLOOPNODE_H

#include <vector>
#include <string>
#include "astnode.h"

class ForLoopNode : public AstNode {
    public:
        static int count;
        ForLoopNode();
        ~ForLoopNode();
        void makeGraph(std::ofstream& outfile);
        AstNodeType nodeType();
        void addChild(AstNode* node);
        std::vector<AstNode*>* getChildren();
        std::vector<AstNode*> mstatements;
        AstNode* getConditional();
        AstNode* getUpdate();
        AstNode* getInit();
        AstNode* getBody();
    private:
        int id;
};



#endif

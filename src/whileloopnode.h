#ifndef WHILELOOPNODE_H
#define WHILELOOPNODE_H

#include <vector>
#include <string>
#include "astnode.h"

class WhileLoopNode : public AstNode {
    public:
        static int count;
        WhileLoopNode();
        ~WhileLoopNode();
        void makeGraph(std::ofstream& outfile);
        AstNodeType type();
        void addChild(AstNode* node);
        std::vector<AstNode*>* getChildren();
        std::vector<AstNode*> mstatements;
    private:
        int id;
};



#endif

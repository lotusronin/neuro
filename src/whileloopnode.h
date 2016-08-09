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
        AstNodeType nodeType();
        void addChild(AstNode* node);
        std::vector<AstNode*>* getChildren();
        std::vector<AstNode*> mstatements;
        AstNode* getConditional();
        AstNode* getBody();
    private:
        int id;
};



#endif

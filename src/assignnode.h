#ifndef ASSIGNNODE_H
#define ASSIGNNODE_H

#include <vector>
#include <string>
#include "astnode.h"

class AssignNode : public AstNode {
    public:
        static int count;
        AssignNode();
        ~AssignNode();
        void makeGraph(std::ofstream& outfile);
        AstNodeType type();
        void addChild(AstNode* node);
        std::vector<AstNode*>* getChildren();
        std::vector<AstNode*> mchildren;
    private:
        int id;
};



#endif

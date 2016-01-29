#ifndef VARDECASSIGNNODE_H
#define VARDECASSIGNNODE_H

#include <vector>
#include <string>
#include "astnode.h"

class VarDecAssignNode : public AstNode {
    public:
        static int count;
        VarDecAssignNode();
        ~VarDecAssignNode();
        void makeGraph(std::ofstream& outfile);
        AstNodeType type();
        void addChild(AstNode* node);
        std::vector<AstNode*> mstatements;
    private:
        int id;
};



#endif

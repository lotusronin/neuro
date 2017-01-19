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
        AstNodeType nodeType();
        void addChild(AstNode* node);
        std::vector<AstNode*>* getChildren();
        std::vector<AstNode*> mchildren;
        AstNode* getLHS();
        AstNode* getRHS();
    private:
        int id;
};



#endif

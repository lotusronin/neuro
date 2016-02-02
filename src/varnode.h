#ifndef VARNODE_H
#define VARNODE_H

#include <vector>
#include <string>
#include "astnode.h"

class VarNode : public AstNode {
    public:
        static int count;
        VarNode();
        ~VarNode();
        void makeGraph(std::ofstream& outfile);
        AstNodeType type();
        void addChild(AstNode* node);
        void addVarName(std::string name);
        std::vector<AstNode*>* getChildren();
        std::vector<AstNode*> mchildren;
    private:
        int id;
        std::string mname;
};

#endif

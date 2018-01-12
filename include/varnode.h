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
        void addChild(AstNode* node);
        void addVarName(std::string name);
        std::string getVarName();
        std::vector<AstNode*>* getChildren();
        int id;
        std::string mname;
};

#endif

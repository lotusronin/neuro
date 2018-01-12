#ifndef CONSTANTNODE_H
#define CONSTANTNODE_H

#include <vector>
#include <string>
#include "astnode.h"

class ConstantNode : public AstNode {
    public:
        static int count;
        ConstantNode();
        ~ConstantNode();
        void addChild(AstNode* node);
        std::vector<AstNode*>* getChildren();
        void setVal(std::string& val);
        void setToken(const Token& t);
        std::string getVal();
        int id;
        std::string mval;
};



#endif

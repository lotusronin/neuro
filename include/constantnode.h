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
        void setVal(const char* val);
        void setToken(const Token& t);
        const char* getVal() const;
        int id;
        const char* mval;
};



#endif

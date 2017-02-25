#ifndef TYPENODE_H
#define TYPENODE_H

#include <vector>
#include <string>
#include "astnode.h"

class TypeNode : public AstNode {
    public:
        static int count;
        TypeNode();
        ~TypeNode();
        AstNodeType nodeType();
        void addChild(AstNode* node);
        void setTypeName(std::string tname);
        bool isPointerType();
        void setToken(const Token& t);
        std::vector<AstNode*>* getChildren();
        std::vector<AstNode*> mchildren;
        int mindirection;
    private:
        int id;
        std::string mname;
};

#endif

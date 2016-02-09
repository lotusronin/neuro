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
        void makeGraph(std::ofstream& outfile);
        AstNodeType nodeType();
        void addChild(AstNode* node);
        void setTypeName(std::string tname);
        void setToken(const Token& t);
        std::vector<AstNode*>* getChildren();
        std::vector<AstNode*> mchildren;
    private:
        int id;
        std::string mname;
};

#endif

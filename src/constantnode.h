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
        void makeGraph(std::ofstream& outfile);
        AstNodeType nodeType();
        void addChild(AstNode* node);
        std::vector<AstNode*>* getChildren();
        void setVal(std::string& val);
        void setToken(const Token& t);
        std::vector<AstNode*> mchildren;
    private:
        int id;
        std::string mval;
};



#endif

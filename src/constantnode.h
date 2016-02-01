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
        AstNodeType type();
        void addChild(AstNode* node);
        void setVal(std::string& val);
        std::vector<AstNode*> mchildren;
    private:
        int id;
        std::string mval;
};



#endif

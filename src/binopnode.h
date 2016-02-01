#ifndef BINOPNODE_H
#define BINOPNODE_H

#include <vector>
#include <string>
#include "astnode.h"

class BinOpNode : public AstNode {
    public:
        static int count;
        BinOpNode();
        ~BinOpNode();
        void makeGraph(std::ofstream& outfile);
        AstNodeType type();
        void addChild(AstNode* node);
        void setOp(std::string& op);
        std::vector<AstNode*> mchildren;
    private:
        int id;
        //TODO(marcus): make this an enum
        std::string mop;
};



#endif

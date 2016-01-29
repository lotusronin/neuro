#ifndef DEFERSTMTNODE_H
#define DEFERSTMTNODE_H

#include <vector>
#include <string>
#include "astnode.h"

class DeferStmtNode : public AstNode {
    public:
        static int count;
        DeferStmtNode();
        ~DeferStmtNode();
        void makeGraph(std::ofstream& outfile);
        AstNodeType type();
        void addChild(AstNode* node);
        std::vector<AstNode*> mchildren;
    private:
        int id;
};

#endif

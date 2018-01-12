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
        void addChild(AstNode* node);
        std::vector<AstNode*>* getChildren();
        int id;
};

#endif

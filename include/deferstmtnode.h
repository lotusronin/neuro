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
        AstNodeType nodeType();
        void addChild(AstNode* node);
        std::vector<AstNode*>* getChildren();
        //std::vector<AstNode*> mchildren;
//    private:
        int id;
};

#endif

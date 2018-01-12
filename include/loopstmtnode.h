#ifndef LOOPSTMTNODE_H
#define LOOPSTMTNODE_H

#include <vector>
#include <string>
#include "astnode.h"

class LoopStmtNode : public AstNode {
    public:
        static int count;
        LoopStmtNode();
        ~LoopStmtNode();
        void addChild(AstNode* node);
        void setBreak(bool b);
        std::vector<AstNode*>* getChildren();
        int id;
        bool isBreak;
};



#endif

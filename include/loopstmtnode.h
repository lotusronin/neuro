#ifndef LOOPSTMTNODE_H
#define LOOPSTMTNODE_H

#include <vector>
#include <string>
#include "astnode.h"

class LoopStmtNode : public AstNode {
    public:
        static int count;
        LoopStmtNode();
        LoopStmtNode(LoopStmtNode* n);
        ~LoopStmtNode();
        int id;
};



#endif

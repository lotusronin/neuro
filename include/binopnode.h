#ifndef BINOPNODE_H
#define BINOPNODE_H

#include <vector>
#include "astnode.h"
#include "tokens.h"

class BinOpNode : public AstNode {
    public:
        static int count;
        static int constructed;
        BinOpNode();
        ~BinOpNode();
        void setOp(const char* op);
        const char* getOp() const;
        AstNode* LHS();
        void setLHS(AstNode* ast);
        AstNode* RHS();
        void setRHS(AstNode* ast);
        void setToken(Token& t);
        SemanticType getType() const;
        static void printDeleted();
        static int deleted;
        const char* mop;
        int id;
        bool unaryOp;
        AstNode* opOverload;
};



#endif

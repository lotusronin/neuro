#ifndef BINOPNODE_H
#define BINOPNODE_H

#include <vector>
#include <string>
#include "astnode.h"
#include "tokens.h"

class BinOpNode : public AstNode {
    public:
        static int count;
        static int constructed;
        BinOpNode();
        ~BinOpNode();
        void setOp(const std::string& op);
        std::string getOp() const;
        AstNode* LHS();
        void setLHS(AstNode* ast);
        AstNode* RHS();
        void setRHS(AstNode* ast);
        void setToken(Token& t);
        SemanticType getType() const;
        static void printDeleted();
        static int deleted;
        //TODO(marcus): make this an enum
        std::string mop;
        int id;
        bool unaryOp;
        AstNode* opOverload;
};



#endif

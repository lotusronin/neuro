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
        AstNodeType nodeType();
        void addChild(AstNode* node);
        std::vector<AstNode*>* getChildren();
        void setOp(const std::string& op);
        std::string getOp();
        AstNode* LHS();
        void setLHS(AstNode* ast);
        AstNode* RHS();
        void setRHS(AstNode* ast);
        int getPriority();
        void setToken(Token& t);
        SemanticType getType();
        static void printDeleted();
        static int deleted;
        //TODO(marcus): make this an enum
        std::string mop;
        int id;
        bool unaryOp;
        AstNode* opOverload;
};



#endif

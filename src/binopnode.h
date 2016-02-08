#ifndef BINOPNODE_H
#define BINOPNODE_H

#include <vector>
#include <string>
#include "astnode.h"
#include "tokens.h"

class BinOpNode : public AstNode {
    public:
        static int count;
        BinOpNode();
        ~BinOpNode();
        void makeGraph(std::ofstream& outfile);
        AstNodeType type();
        void addChild(AstNode* node);
        std::vector<AstNode*>* getChildren();
        void setOp(std::string& op);
        std::string getOp();
        AstNode* LHS();
        void setLHS(AstNode* ast);
        AstNode* RHS();
        void setRHS(AstNode* ast);
        int getPriority();
        void setToken(Token& t);
        std::vector<AstNode*> mchildren;
    private:
        int id;
        //TODO(marcus): make this an enum
        std::string mop;
        int mpriority;
};



#endif

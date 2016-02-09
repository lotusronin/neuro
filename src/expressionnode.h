#ifndef EXPRESSIONNODE_H
#define EXPRESSIONNODE_H

#include <vector>
#include <string>
#include "astnode.h"

class ExpressionNode : public AstNode {
    public:
        static int count;
        ExpressionNode();
        ~ExpressionNode();
        void makeGraph(std::ofstream& outfile);
        AstNodeType nodeType()();
        void addChild(AstNode* node);
        void setExprKind(std::string& kind);
        std::vector<AstNode*> mchildren;
    private:
        int id;
        std::string mexprkind;
};

#endif

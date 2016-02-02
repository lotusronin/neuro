#ifndef FUNCCALLNODE_H
#define FUNCCALLNODE_H

#include <vector>
#include <string>
#include "astnode.h"

class FuncCallNode : public AstNode {
    public:
        static int count;
        FuncCallNode();
        ~FuncCallNode();
        void makeGraph(std::ofstream& outfile);
        AstNodeType type();
        void addArgs(AstNode* node);
        void addChild(AstNode* node);
        void addFuncName(std::string funcname);
        std::vector<AstNode*>* getChildren();
        std::vector<AstNode*> margs;
        std::string mfuncname;
    private:
        int id;
};



#endif

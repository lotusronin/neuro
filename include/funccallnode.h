#ifndef FUNCCALLNODE_H
#define FUNCCALLNODE_H

#include <vector>
#include <string>
#include "astnode.h"
#include "funcdefnode.h"

class FuncCallNode : public AstNode {
    public:
        static int count;
        FuncCallNode();
        FuncCallNode(FuncCallNode* n);
        ~FuncCallNode();
        void addFuncName(std::string funcname);
        int id;
        std::string mfuncname;
        std::string scopes;
        FuncDefNode* func;
};



#endif

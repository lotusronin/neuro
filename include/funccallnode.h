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
        void addChild(AstNode* node);
        void addFuncName(std::string funcname);
        std::vector<AstNode*>* getChildren();
        std::string mfuncname;
        std::string mfuncnamemangled;
        int id;
        std::string scopes;
};



#endif

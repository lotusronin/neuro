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
        AstNodeType nodeType();
        void addArgs(AstNode* node);
        void addChild(AstNode* node);
        void addFuncName(std::string funcname);
        std::vector<AstNode*>* getChildren();
        //std::vector<AstNode*> margs;
        std::string mfuncname;
        std::string mfuncnamemangled;
    //private:
        int id;
        std::string scopes;
};



#endif

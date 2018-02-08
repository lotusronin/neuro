#ifndef FUNCDEFNODE_H
#define FUNCDEFNODE_H

#include <vector>
#include <string>
#include "astnode.h"
#include "arrayview.h"

class FuncDefNode : public AstNode {
    public:
        static int count;
        FuncDefNode(AstNodeType ntype);
        FuncDefNode(FuncDefNode* n);
        ~FuncDefNode();
        void addFuncName(std::string funcname);
        ArrayView getParameters();
        AstNode* getFunctionBody();
        std::string mangledName();
        std::string mfuncname;
        char* op;
        int isOperatorOverload;
        int isTemplated;
        int id;
};



#endif

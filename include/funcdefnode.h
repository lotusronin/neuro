#ifndef FUNCDEFNODE_H
#define FUNCDEFNODE_H

#include <vector>
#include <string>
#include "astnode.h"
#include "arrayview.h"

class FuncDefNode : public AstNode {
    public:
        static int count;
        FuncDefNode();
        ~FuncDefNode();
        AstNodeType nodeType();
        void addParams(AstNode* node);
        void addChild(AstNode* node);
        void addFuncName(std::string funcname);
        SemanticType getType();
        std::vector<AstNode*>* getChildren();
        ArrayView getParameters();
        AstNode* getFunctionBody();
        std::string mangledName();
        //std::vector<AstNode*> mparams;
        std::string mfuncname;
        int isOperatorOverload;
        char* op;
    //private:
        int id;
};



#endif

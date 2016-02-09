#ifndef FUNCDEFNODE_H
#define FUNCDEFNODE_H

#include <vector>
#include <string>
#include "astnode.h"

class FuncDefNode : public AstNode {
    public:
        static int count;
        FuncDefNode();
        ~FuncDefNode();
        void makeGraph(std::ofstream& outfile);
        AstNodeType nodeType();
        void addParams(AstNode* node);
        void addChild(AstNode* node);
        void addFuncName(std::string funcname);
        SemanticType getType();
        std::vector<AstNode*>* getChildren();
        std::vector<AstNode*> mparams;
        std::string mfuncname;
    private:
        int id;
};



#endif

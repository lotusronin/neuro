#ifndef PROTOTYPENODE_H
#define PROTOTYPENODE_H

#include <vector>
#include <string>
#include "astnode.h"
#include "arrayview.h"

class PrototypeNode : public AstNode {
    public:
        static int count;
        PrototypeNode();
        ~PrototypeNode();
        AstNodeType nodeType();
        void addParams(AstNode* node);
        void addChild(AstNode* node);
        void addFuncName(std::string funcname);
        std::vector<AstNode*>* getChildren();
        ArrayView getParameters();
        SemanticType getType();
        //std::vector<AstNode*> mparams;
        std::string mfuncname;
    //private:
        int id;
};



#endif

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
        void addChild(AstNode* node);
        void addFuncName(std::string funcname);
        std::vector<AstNode*>* getChildren();
        ArrayView getParameters();
        SemanticType getType();
        std::string mfuncname;
        int id;
};



#endif

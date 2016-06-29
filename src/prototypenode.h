#ifndef PROTOTYPENODE_H
#define PROTOTYPENODE_H

#include <vector>
#include <string>
#include "astnode.h"

class PrototypeNode : public AstNode {
    public:
        static int count;
        PrototypeNode();
        ~PrototypeNode();
        void makeGraph(std::ofstream& outfile);
        AstNodeType nodeType();
        void addParams(AstNode* node);
        void addChild(AstNode* node);
        void addFuncName(std::string funcname);
        std::vector<AstNode*>* getChildren();
        std::vector<AstNode*>* getParameters();
        SemanticType getType();
        std::vector<AstNode*> mparams;
        std::string mfuncname;
    private:
        int id;
};



#endif
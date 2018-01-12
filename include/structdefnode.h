#ifndef STRUCTDEFNODE_H
#define STRUCTDEFNODE_H

#include <vector>
#include <string>
#include "astnode.h"

class StructDefNode : public AstNode {
    public:
        static int count;
        StructDefNode();
        ~StructDefNode();
        void addChild(AstNode* node);
        std::string& getIdent();
        std::vector<AstNode*>* getChildren();
        std::string ident;
        bool foreign;
        int id;
};

#endif //STRUCTDEFNODE_H
